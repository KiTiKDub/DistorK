/*
  ==============================================================================

    SaturationData.cpp
    Created: 17 Nov 2023 4:43:11pm
    Author:  kylew

  ==============================================================================
*/

#include "SaturationData.h"

void Saturation::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
    inGain.reset();
    inGain.prepare(spec);
    inGain.setRampDurationSeconds(.05);

    outGain.reset();
    outGain.prepare(spec);
    outGain.setRampDurationSeconds(.05);
}

void Saturation::process(juce::dsp::AudioBlock<float>& block, int channel)
{
    if (satBypass) { return; };

    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    auto channelInput = context.getInputBlock().getChannelPointer(channel);
    auto channelOutput = context.getOutputBlock().getChannelPointer(channel);

    inGain.setGainDecibels(satInGain);
    inGain.process(context);

    for (int s = 0; s < context.getInputBlock().getNumSamples(); s++)
    {
        if (channelInput[s] != 0)
        {
            auto power = pow(channelInput[s], 2) / abs(channelInput[s]) * satDrive;
            auto distort = (channelInput[s] / abs(channelInput[s])) * (1 - std::exp(-power));
            channelOutput[s] = distort * satMix + ((1 - satMix) * channelInput[s]);
        }
        
    }

    outGain.setGainDecibels(satOutGain);
    outGain.process(context);
}

void Saturation::updateParams(bool bypass, float drive, float inGain, float outGain, float mix)
{
    satBypass = bypass;
    satDrive = drive;
    satInGain = inGain;
    satOutGain = outGain;
    satMix = mix/100;
}

