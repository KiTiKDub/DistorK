/*
  ==============================================================================

    BitCrusherData.h
    Created: 17 Nov 2023 4:42:41pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utility/overSampleGain.h"

struct BitCrusher
{
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block);
    void updateParams(bool bypass, int bitDepth, int bitRate, float inGain, float outGain, float mix);

private:
    
    overSampleGain osg;

    bool crusherBypass;
    int crusherBitDepth;
    int crusherBitRate;
    float crusherInGain;
    float crusherOutGain;
    float crusherMix;
};