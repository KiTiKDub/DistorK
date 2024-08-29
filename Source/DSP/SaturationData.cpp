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
}

void Saturation::process(juce::dsp::AudioBlock<float>& block)
{
    if (satBypass) { return; };

    for (int channel = 0; channel < block.getNumChannels(); channel++)
    {
        osg.process(block, satInGain, channel);

        auto data = block.getChannelPointer(channel);

        for (int s = 0; s < block.getNumSamples(); s++)
        {
            if (data[s] != 0)
            {
                auto power = pow(data[s], 2) / abs(data[s]) * satDrive;
                auto distort = (data[s] / abs(data[s])) * (1 - std::exp(-power));
                data[s] = distort * satMix + ((1 - satMix) * data[s]);
            }

        }

        osg.process(block, satOutGain, channel);
    }

}

void Saturation::updateParams(bool bypass, float drive, float inGain, float outGain, float mix)
{
    satBypass = bypass;
    satDrive = drive;
    satInGain = inGain;
    satOutGain = outGain;
    satMix = mix/100;
}

