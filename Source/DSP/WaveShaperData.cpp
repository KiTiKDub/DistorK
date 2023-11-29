/*
  ==============================================================================

    WaveShaperData.cpp
    Created: 17 Nov 2023 4:42:05pm
    Author:  kylew

  ==============================================================================
*/

#include "WaveShaperData.h"

void WaveShaper::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
    inGain.reset();
    inGain.prepare(spec);
    inGain.setRampDurationSeconds(.05);

    outGain.reset();
    outGain.prepare(spec);
    outGain.setRampDurationSeconds(.05);
}

void WaveShaper::process(juce::dsp::AudioBlock<float>& block, int channel)
{
    if (waveShaperBypass) { return; };

    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    inGain.setGainDecibels(waveShaperInGainValue);
    inGain.process(context);

    for (int channel = 0; channel < block.getNumChannels(); ++channel)
    {
        switch (waveShaperTypeSelect)
        {
        case WaveShaper::sinusoidal:
            processSinusoidal(channel, context);
            break;

        case WaveShaper::quadratic:
            processQuadratic(channel, context);
            break;

        case WaveShaper::factor:
            processFactor(channel, context);
            break;

        case WaveShaper::GloubiBoulga:
            processGB(channel, context);
            break;
        }

    }

    outGain.setGainDecibels(waveShaperOutGainValue);
    outGain.process(context);
}

void WaveShaper::updateParams(bool bypass, int typeSelect, std::vector<juce::AudioParameterFloat*>& factors, float inGain, float outGain, float mix)
{
    waveShaperBypass = bypass;
    waveShaperTypeSelect = typeSelect;
    waveShaperFactors = factors;
    waveShaperInGainValue = inGain;
    waveShaperOutGainValue = outGain;
    waveshaperMix = mix/100;
}

void WaveShaper::processSinusoidal(int channel, juce::dsp::ProcessContextReplacing<float>& context)
{
    auto factor = waveShaperFactors[0]->get();

    auto z = juce::MathConstants<float>::pi * factor;
    auto a = (1 / sin(z));
    auto b = 1 / factor;

    auto* channelInput = context.getInputBlock().getChannelPointer(channel);
    auto* channelOutput = context.getOutputBlock().getChannelPointer(channel);

    for (int s = 0; s < context.getInputBlock().getNumSamples(); ++s)
    {
        if (channelInput[s] > b)
        {
            channelOutput[s] = (1 * waveshaperMix) + (channelInput[s] * (1-waveshaperMix));
        }
        else
        {
            channelOutput[s] = ((sin(z * channelInput[s]) * a) * waveshaperMix) + (channelInput[s] * (1 - waveshaperMix));
        }

    }
}

void WaveShaper::processQuadratic(int channel, juce::dsp::ProcessContextReplacing<float>& context)
{
    auto* channelInput = context.getInputBlock().getChannelPointer(channel);
    auto* channelOutput = context.getOutputBlock().getChannelPointer(channel);

    auto factor = waveShaperFactors[1]->get();

    for (int s = 0; s < context.getInputBlock().getNumSamples(); ++s)
    {
        auto shape = channelInput[s] * (abs(channelInput[s]) + factor) / (pow(channelInput[s], 2) + (factor - 1) * abs(channelInput[s]) + 1);
        channelOutput[s] = (shape * waveshaperMix) + (channelInput[s] * (1-waveshaperMix));
    }
}

void WaveShaper::processFactor(int channel, juce::dsp::ProcessContextReplacing<float>& context)
{
    auto* channelInput = context.getInputBlock().getChannelPointer(channel);
    auto* channelOutput = context.getOutputBlock().getChannelPointer(channel);

    auto vectorFactor = waveShaperFactors[2]->get();
    auto factor = 2 * vectorFactor / (1 - vectorFactor);

    for (int s = 0; s < context.getInputBlock().getNumSamples(); ++s)
    {
        auto shape = ((1 + factor) * channelInput[s]) / (1 + factor * abs(channelInput[s]));
        channelOutput[s] = (shape * waveshaperMix) + (channelInput[s] * (1 - waveshaperMix));
    }
}

void WaveShaper::processGB(int channel, juce::dsp::ProcessContextReplacing<float>& context)
{
    auto* channelInput = context.getInputBlock().getChannelPointer(channel);
    auto* channelOutput = context.getOutputBlock().getChannelPointer(channel);

    auto factor = waveShaperFactors[3]->get();

    for (int s = 0; s < context.getInputBlock().getNumSamples(); ++s)
    {
        auto distort = channelInput[s] * factor;
        auto constant = 1 + exp(sqrt(fabs(distort)) * -0.75);

        auto shape = (exp(distort) - exp(-distort * constant)) / (exp(distort) + exp(-distort));
        channelOutput[s] = (shape * waveshaperMix) + (channelInput[s] * (1 - waveshaperMix));
    }
}
