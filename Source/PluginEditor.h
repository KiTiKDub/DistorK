/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/kLookAndFeel.h"
#include "GUI/rotarySliderWithLabels.h"

//==============================================================================
/**
*/
class DistorKAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DistorKAudioProcessorEditor (DistorKAudioProcessor&);
    ~DistorKAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void attachRSWL();
    void updateToggleState(juce::Button* button);

private:
    
    DistorKAudioProcessor& audioProcessor;

    Laf lnf;
    juce::Image logo;
    juce::Font newFont;

    std::unique_ptr<RotarySliderWithLabels> oversampleSelect;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oversampleSelectAT;

    juce::ToggleButton selectClip, selectBit, selectWaveShpr, selectSat;
    juce::AudioProcessorValueTreeState::ButtonAttachment selectClipAT, selectBitAT, selectWaveShprAT, selectSatAT;

    std::vector<juce::String> oversamplingText{ "1x", "2x", "4x", "8x" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistorKAudioProcessorEditor)
};
