/*
  ==============================================================================

    overSampleGain.cpp
    Created: 28 Aug 2024 4:42:37pm
    Author:  kylew

  ==============================================================================
*/

#include "overSampleGain.h"

void overSampleGain::process(juce::dsp::AudioBlock<float>& block, float setGain, int channel)
{
    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); s++) //In Gain
    {
        data[s] *= juce::Decibels::decibelsToGain(setGain);
    }
}
