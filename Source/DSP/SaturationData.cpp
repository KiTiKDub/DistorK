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

void Saturation::process(juce::dsp::AudioBlock<float>& block)
{
    if (satBypass) { return; };

    /*inGain.setGainDecibels(satInGain);
    inGain.process(context);*/

    //auto ovBlock = overSamplers[ovRate].processSamplesUp(context.getInputBlock());

    for (int channel = 0; channel < block.getNumChannels(); channel++)
    {
        auto data = block.getChannelPointer(channel);

        for (int s = 0; s < block.getNumSamples(); s++) //In Gain
        {
            data[s] *= juce::Decibels::decibelsToGain(satInGain);
        }

        for (int s = 0; s < block.getNumSamples(); s++)
        {
            if (data[s] != 0)
            {
                auto power = pow(data[s], 2) / abs(data[s]) * satDrive;
                auto distort = (data[s] / abs(data[s])) * (1 - std::exp(-power));
                data[s] = distort * satMix + ((1 - satMix) * data[s]);
            }

        }

    }

    //overSamplers[ovRate].processSamplesDown(context.getOutputBlock());

    /*outGain.setGainDecibels(satOutGain);
    outGain.process(context);*/

}

void Saturation::updateParams(bool bypass, float drive, float inGain, float outGain, float mix)
{
    satBypass = bypass;
    satDrive = drive;
    satInGain = inGain;
    satOutGain = outGain;
    satMix = mix/100;
}

