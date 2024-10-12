/*
  ==============================================================================

    BitCrusher.h
    Created: 17 Nov 2023 4:40:15pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "rotarySliderWithLabels.h"

struct BitCrusherComp : public juce::Component
{
    BitCrusherComp(juce::AudioProcessorValueTreeState& apvts);
    ~BitCrusherComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:

    void updateRSWL(juce::AudioProcessorValueTreeState& apvts);

    std::unique_ptr<RotarySliderWithLabels> depth, rate, inGain, mix, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAT, rateAT, inGainAT, mixAT, outGainAT;
};