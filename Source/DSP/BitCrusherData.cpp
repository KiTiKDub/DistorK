/*
  ==============================================================================

    BitCrusherData.cpp
    Created: 17 Nov 2023 4:42:41pm
    Author:  kylew

  ==============================================================================
*/

#include "BitCrusherData.h"

void BitCrusher::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
}

void BitCrusher::process(juce::dsp::AudioBlock<float>& block)
{
    if (crusherBypass) { return; };

    for (int channel = 0; channel < block.getNumChannels(); channel++)
    {
        osg.process(block, crusherInGain, channel);

        auto data = block.getChannelPointer(channel);

        for (int s = 0; s < block.getNumSamples(); ++s)
        {
            auto input = data[s];
            auto crusher = pow(2, crusherBitDepth);
            auto crushedData = floor(crusher * input) / crusher;

            data[s] = (crushedData * crusherMix) + (input * (1 - crusherMix));

            if (crusherBitRate > 1)
            {
                if (s % crusherBitRate != 0)
                {
                    auto redux = data[s - s % crusherBitRate];
                    data[s] = (redux * crusherMix) + (input * (1 - crusherMix));
                }
            }
        }

        osg.process(block, crusherOutGain, channel);
    }
}

void BitCrusher::updateParams(bool bypass, int bitDepth, int bitRate, float inGain, float outGain, float mix)
{
    crusherBypass = bypass;
    crusherBitDepth = bitDepth;
    crusherBitRate = bitRate;
    crusherInGain = inGain;
    crusherOutGain = outGain;
    crusherMix = mix/100;
}
