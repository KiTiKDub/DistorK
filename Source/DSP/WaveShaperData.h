/*
  ==============================================================================

    WaveShaperData.h
    Created: 17 Nov 2023 4:42:05pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
struct WaveShaper 
{
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block, int channel);
    void updateParams(int typeSelect, std::vector<float>& factors, float inGain, float outGain, float mix);

private:

    void processSinusoidal(int channel, juce::dsp::ProcessContextReplacing<float>& context);
    void processQuadratic(int channel, juce::dsp::ProcessContextReplacing<float>& context);
    void processFactor(int channel, juce::dsp::ProcessContextReplacing<float>& context);
    void processGB(int channel, juce::dsp::ProcessContextReplacing<float>& context);

    enum WaveShaperSelect {
        none,
        sinusoidal,
        quadratic,
        factor,
        GloubiBoulga
    };

    juce::dsp::Gain<float> inGain;
    juce::dsp::Gain<float> outGain;

    int waveShaperTypeSelect;
    std::vector<float> waveShaperFactors;
    float waveShaperInGainValue;
    float waveShaperOutGainValue;
    float waveshaperMix;
};