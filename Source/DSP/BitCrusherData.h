/*
  ==============================================================================

    BitCrusherData.h
    Created: 17 Nov 2023 4:42:41pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct BitCrusher
{
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::ProcessContextReplacing<float>& context, int ovRate, std::array<juce::dsp::Oversampling<float>, 4>& oversampelers);
    void updateParams(bool bypass, int bitDepth, int bitRate, float inGain, float outGain, float mix);

private:
    
    juce::dsp::Gain<float> inGain;
    juce::dsp::Gain<float> outGain;

    bool crusherBypass;
    int crusherBitDepth;
    int crusherBitRate;
    float crusherInGain;
    float crusherOutGain;
    float crusherMix;
};