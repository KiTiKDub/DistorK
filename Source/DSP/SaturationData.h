/*
  ==============================================================================

    SaturationData.h
    Created: 17 Nov 2023 4:43:11pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utility/overSampleGain.h"

struct Saturation
{
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block);
    void updateParams(bool bypass, float drive, float inGain, float outGain, float mix);

private:

    overSampleGain osg;

    bool satBypass;
    float satDrive;
    float satInGain;
    float satOutGain;
    float satMix;
};
