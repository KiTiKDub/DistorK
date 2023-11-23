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
    inGain.reset();
    inGain.prepare(spec);
    inGain.setRampDurationSeconds(.05);

    outGain.reset();
    outGain.prepare(spec);
    outGain.setRampDurationSeconds(.05);
}

void BitCrusher::process(juce::dsp::AudioBlock<float>& block, int channel)
{
    if (crusherBypass) { return; };

    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    auto channelInput = context.getInputBlock().getChannelPointer(channel);
    auto channelOutput = context.getOutputBlock().getChannelPointer(channel);

    inGain.setGainDecibels(crusherInGain);
    inGain.process(context);

    for (int s = 0; s < context.getInputBlock().getNumSamples(); ++s)
    {
        auto input = channelInput[s];
        auto crusher = pow(2, crusherBitDepth);
        auto crushedData = floor(crusher * input) / crusher;

        channelOutput[s] = (crushedData * crusherMix) + (input * (1 - crusherMix));

        if (crusherBitRate > 1)
        {
            if (s % crusherBitRate != 0)
            {
                auto redux = channelOutput[s - s % crusherBitRate];
                channelOutput[s] = (redux * crusherMix) + (input * (1 - crusherMix));
            }
        }
    }

    outGain.setGainDecibels(crusherOutGain);
    outGain.process(context);
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
