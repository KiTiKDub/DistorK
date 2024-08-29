/*
  ==============================================================================

    overSampleGain.h
    Created: 28 Aug 2024 4:42:37pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct overSampleGain
{
    void process(juce::dsp::AudioBlock<float>& block, float setGain, int channel);
};