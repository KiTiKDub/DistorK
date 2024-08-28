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

void WaveShaper::process(juce::dsp::AudioBlock<float>& block)
{
    if (waveShaperBypass) { return; };

    /*inGain.setGainDecibels(waveShaperInGainValue);
    inGain.process(context);*/

    //auto ovBlock = overSamplers[ovRate].processSamplesUp(context.getInputBlock());

    for (int channel = 0; channel < block.getNumChannels(); channel++)
    {
        switch (waveShaperTypeSelect)
        {
        case WaveShaper::sinusoidal:
            processSinusoidal(channel, block);
            break;

        case WaveShaper::quadratic:
            processQuadratic(channel, block);
            break;

        case WaveShaper::factor:
            processFactor(channel, block);
            break;

        case WaveShaper::GloubiBoulga:
            processGB(channel, block);
            break;
        }

    }

    //overSamplers[ovRate].processSamplesDown(context.getOutputBlock());

    /*outGain.setGainDecibels(waveShaperOutGainValue);
    outGain.process(context);*/
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

void WaveShaper::processSinusoidal(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto factor = waveShaperFactors[0]->get();

    auto z = juce::MathConstants<float>::pi * factor;
    auto a = (1 / sin(z));
    auto b = 1 / factor;

    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); s++) //In Gain
    {
        data[s] *= juce::Decibels::decibelsToGain(waveShaperInGainValue);
    }

    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        if (data[s] > b)
        {
            data[s] = (1 * waveshaperMix) + (data[s] * (1-waveshaperMix));
        }
        else
        {
            data[s] = ((sin(z * data[s]) * a) * waveshaperMix) + (data[s] * (1 - waveshaperMix));
        }

    }

    for (int s = 0; s < block.getNumSamples(); s++) //out Gain
    {
        data[s] *= juce::Decibels::decibelsToGain(waveShaperOutGainValue);
    }
}

void WaveShaper::processQuadratic(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto factor = waveShaperFactors[1]->get();

    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); s++) //In Gain
    {
        data[s] *= juce::Decibels::decibelsToGain(waveShaperInGainValue);
    }

    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        auto shape = data[s] * (abs(data[s]) + factor) / (pow(data[s], 2) + (factor - 1) * abs(data[s]) + 1);
        data[s] = (shape * waveshaperMix) + (data[s] * (1-waveshaperMix));
    }

    for (int s = 0; s < block.getNumSamples(); s++) //Out Gain
    {
        data[s] *= juce::Decibels::decibelsToGain(waveShaperOutGainValue);
    }
}

void WaveShaper::processFactor(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto vectorFactor = waveShaperFactors[2]->get();
    auto factor = 2 * vectorFactor / (1 - vectorFactor);

    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); s++) //In Gain
    {
        data[s] *= juce::Decibels::decibelsToGain(waveShaperInGainValue);
    }

    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        auto shape = ((1 + factor) * data[s]) / (1 + factor * abs(data[s]));
        data[s] = (shape * waveshaperMix) + (data[s] * (1 - waveshaperMix));
    }

    for (int s = 0; s < block.getNumSamples(); s++) //Out Gain
    {
        data[s] *= juce::Decibels::decibelsToGain(waveShaperOutGainValue);
    }
}

void WaveShaper::processGB(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto factor = waveShaperFactors[3]->get();

    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); s++) //In Gain
    {
        data[s] *= juce::Decibels::decibelsToGain(waveShaperInGainValue);
    }

    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        auto distort = data[s] * factor;
        auto constant = 1 + exp(sqrt(fabs(distort)) * -0.75);

        auto shape = (exp(distort) - exp(-distort * constant)) / (exp(distort) + exp(-distort));
        data[s] = (shape * waveshaperMix) + (data[s] * (1 - waveshaperMix));
    }

    for (int s = 0; s < block.getNumSamples(); s++) //out Gain
    {
        data[s] *= juce::Decibels::decibelsToGain(waveShaperOutGainValue);
    }
}
