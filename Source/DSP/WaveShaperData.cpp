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
}

void WaveShaper::process(juce::dsp::AudioBlock<float>& block)
{
    if (waveShaperBypass) { return; };

    for (int channel = 0; channel < block.getNumChannels(); channel++)
    {
        osg.process(block, waveShaperInGainValue, channel);

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

        osg.process(block, waveShaperOutGainValue, channel);
    }

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
}

void WaveShaper::processQuadratic(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto factor = waveShaperFactors[1]->get();

    auto data = block.getChannelPointer(channel);


    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        auto shape = data[s] * (abs(data[s]) + factor) / (pow(data[s], 2) + (factor - 1) * abs(data[s]) + 1);
        data[s] = (shape * waveshaperMix) + (data[s] * (1-waveshaperMix));
    }

}

void WaveShaper::processFactor(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto vectorFactor = waveShaperFactors[2]->get();
    auto factor = 2 * vectorFactor / (1 - vectorFactor);

    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        auto shape = ((1 + factor) * data[s]) / (1 + factor * abs(data[s]));
        data[s] = (shape * waveshaperMix) + (data[s] * (1 - waveshaperMix));
    }
}

void WaveShaper::processGB(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto factor = waveShaperFactors[3]->get();

    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        auto distort = data[s] * factor;
        auto constant = 1 + exp(sqrt(fabs(distort)) * -0.75);

        auto shape = (exp(distort) - exp(-distort * constant)) / (exp(distort) + exp(-distort));
        data[s] = (shape * waveshaperMix) + (data[s] * (1 - waveshaperMix));
    }
}
