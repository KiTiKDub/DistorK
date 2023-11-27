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

struct ClipperComp : public juce::Component
{
    ClipperComp(juce::AudioProcessorValueTreeState& apvts);
    ~ClipperComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:

    void updateRSWL(juce::AudioProcessorValueTreeState& apvts);
    void setVertSlider(juce::Slider& slider);

    std::unique_ptr<RotarySliderWithLabels> select, inGain, mix, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> selectAT, inGainAT, mixAT, outGainAT;

    juce::Slider threshold{ "Threshold" };
    juce::AudioProcessorValueTreeState::SliderAttachment thresholdAT;
};