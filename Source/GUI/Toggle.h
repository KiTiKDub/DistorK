/*
  ==============================================================================

    Toggle.h
    Created: 17 Nov 2023 4:41:18pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "rotarySliderWithLabels.h"

struct ToggleComp : public juce::Component
{
    ToggleComp(juce::AudioProcessorValueTreeState& apvts);
    ~ToggleComp();;

    void paint(juce::Graphics& g) override;
    void resized() override;
    juce::ToggleButton selectClip{ "Clipper" }, selectBit{ "Bit Crusher" }, selectWaveShpr{ "WaveShaper" }, selectSat{ "Saturator" };

private:

    void attachRSWL(juce::AudioProcessorValueTreeState& apvts);
    void updateToggleState(juce::Button* button);

    std::unique_ptr<RotarySliderWithLabels> oversampleSelect;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oversampleSelectAT;

    
    juce::ToggleButton toggleClip, toggleBit, toggleWaveShpr, toggleSat;
    juce::AudioProcessorValueTreeState::ButtonAttachment selectClipAT, selectBitAT, selectWaveShprAT, selectSatAT;
    juce::AudioProcessorValueTreeState::ButtonAttachment toggleClipAT, toggleBitAT, toggleWaveShprAT, toggleSatAT;

    std::vector<juce::String> oversamplingText{ "1x", "2x", "4x", "8x" };
};