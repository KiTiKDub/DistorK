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
#include "SliderWithLabels.h"

struct MasterComp : public juce::Component, juce::Timer
{
    MasterComp(DistorKAudioProcessor&);
    ~MasterComp();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:

    DistorKAudioProcessor& audioP;

    void setHorzSlider(juce::Slider& slider);
    void setVertSlider(juce::Slider& slider);
    void updateSWL(juce::AudioProcessorValueTreeState& apvts);

    std::array<LevelMeter, 2> meter;
    std::array<LevelMeter, 2> outMeter;

    std::unique_ptr<SliderWithLabels> inGain, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainAT, outGainAT;

    juce::Slider mix;
    juce::AudioProcessorValueTreeState::SliderAttachment mixAT;

    juce::ToggleButton bypass{ "Bypass" };
    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAT;
};