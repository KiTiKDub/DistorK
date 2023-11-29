/*
  ==============================================================================

    Clipper.h
    Created: 17 Nov 2023 4:40:46pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "rotarySliderWithLabels.h"
#include "SliderWithLabels.h"

struct ClipperComp : public juce::Component
{
    ClipperComp(juce::AudioProcessorValueTreeState& apvts);
    ~ClipperComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:

    void updateRSWL(juce::AudioProcessorValueTreeState& apvts);
    void updateSWL(juce::AudioProcessorValueTreeState& apvts);

    std::unique_ptr<RotarySliderWithLabels> select, inGain, mix, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> selectAT, inGainAT, mixAT, outGainAT;

    std::unique_ptr<SliderWithLabels> threshold;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAT;

    std::vector<juce::String> typeText{ "Hard", "Soft", "Sine", "Hyperbolic Tan", "Arc Tan", "Quintic"};
};