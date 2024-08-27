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

void BitCrusher::process(juce::dsp::ProcessContextReplacing<float>& context, int ovRate, std::array<juce::dsp::Oversampling<float>, 4>& overSamplers)
{
    if (crusherBypass) { return; };



    inGain.setGainDecibels(crusherInGain);
    inGain.process(context);

    auto ovBlock = overSamplers[ovRate].processSamplesUp(context.getInputBlock());

    for (int channel = 0; channel < ovBlock.getNumChannels(); channel++)
    {
        auto data = ovBlock.getChannelPointer(channel);

        for (int s = 0; s < ovBlock.getNumSamples(); ++s)
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
    }

    overSamplers[ovRate].processSamplesDown(context.getOutputBlock());

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
