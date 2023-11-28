/*
  ==============================================================================

    MasterControls.h
    Created: 17 Nov 2023 4:39:37pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "kLevelMeter.h"

struct MasterComp : public juce::Component, juce::Timer
{
    MasterComp(DistorKAudioProcessor&);
    ~MasterComp();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:

    void setHorzSlider(juce::Slider& slider);
    void setVertSlider(juce::Slider& slider);

    DistorKAudioProcessor& audioP;

    std::array<LevelMeter, 2> meter;
    std::array<LevelMeter, 2> outMeter;

    juce::Slider inGain{ "In Gain" }, mix, outGain{ "Out Gain" };
    juce::AudioProcessorValueTreeState::SliderAttachment inGainAT, mixAT, outGainAT;

    juce::ToggleButton bypass{ "Bypass" };
    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAT;
};