/*
  ==============================================================================

    WaveShaperData.h
    Created: 17 Nov 2023 4:42:05pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utility/overSampleGain.h"

struct WaveShaper 
{
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block);
    void updateParams(bool bypass, int typeSelect, std::vector<juce::AudioParameterFloat*>& factors, float inGain, float outGain, float mix);

private:

    void processSinusoidal(int channel, juce::dsp::AudioBlock<float>& block);
    void processQuadratic(int channel, juce::dsp::AudioBlock<float>& block);
    void processFactor(int channel, juce::dsp::AudioBlock<float>& block);
    void processGB(int channel, juce::dsp::AudioBlock<float>& block);

    enum WaveShaperSelect {
        sinusoidal,
        quadratic,
        factor,
        GloubiBoulga
    };

    overSampleGain osg;

    bool waveShaperBypass;
    int waveShaperTypeSelect;
    std::vector<juce::AudioParameterFloat*> waveShaperFactors;
    float waveShaperInGainValue;
    float waveShaperOutGainValue;
    float waveshaperMix;
};