/*
  ==============================================================================

    clipper.h
    Created: 2 Nov 2023 1:42:41pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utility/overSampleGain.h"

struct Clipper
{
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block);
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

    void processHardClipper(int channel, juce::dsp::AudioBlock<float>& block);
    void processCubic(int channel, juce::dsp::AudioBlock<float>& block);
    void processSin(int channel, juce::dsp::AudioBlock<float>& block);
    void processHyperTangent(int channel, juce::dsp::AudioBlock<float>& block);
    void processArcTangent(int channel, juce::dsp::AudioBlock<float>& block);
    void processQuintic(int channel, juce::dsp::AudioBlock<float>& block);

    overSampleGain osg;

    bool clipperBypass;
    int clipperMode;
    float clipperThresh;
    float clipperGainIn;
    float clipperGainOut;
    float clipperMix;
};
