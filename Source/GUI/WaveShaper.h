/*
  ==============================================================================

    WaveShaper.h
    Created: 17 Nov 2023 4:40:32pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "rotarySliderWithLabels.h"

struct WaveShaperComp : public juce::Component
{
    WaveShaperComp(juce::AudioProcessorValueTreeState& apvts);
    ~WaveShaperComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:

    void updateRSWL(juce::AudioProcessorValueTreeState& apvts);
    void updateAttachments(juce::AudioProcessorValueTreeState& apvts);

    std::unique_ptr<RotarySliderWithLabels> select, distort, inGain, mix, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> selectAT, distortAT, inGainAT, mixAT, outGainAT;
};