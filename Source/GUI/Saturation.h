/*
  ==============================================================================

    Saturation.h
    Created: 17 Nov 2023 4:41:53pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "rotarySliderWithLabels.h"

struct SaturationComp : public juce::Component
{
    SaturationComp(juce::AudioProcessorValueTreeState& apvts);
    ~SaturationComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:

    void updateRSWL(juce::AudioProcessorValueTreeState& apvts);

    std::unique_ptr<RotarySliderWithLabels> drive, inGain, outGain, mix;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAT, inGainAT, outGainAT, mixAT;
};