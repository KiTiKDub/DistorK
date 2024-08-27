/*
  ==============================================================================

    clipper.h
    Created: 2 Nov 2023 1:42:41pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct Clipper
{
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::ProcessContextReplacing<float>& block, int ovRate, std::array<juce::dsp::Oversampling<float>, 4>& oversampelers);
    void updateParams(bool bypass, int mode, float threshold, float gainIn, float gainOut, float mix);

private:

    enum
    {
        hardClipper,
        cubic,
        sin,
        hTangent,
        arcTangent,
        quintic
    };

    juce::dsp::Gain<float> inGain;
    juce::dsp::Gain<float> outGain;

    bool clipperBypass;
    int clipperMode;
    float clipperThresh;
    float clipperGainIn;
    float clipperGainOut;
    float clipperMix;
};
