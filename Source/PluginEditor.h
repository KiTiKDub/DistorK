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
#include "GUI/Toggle.h"
#include "GUI/Saturation.h"
#include "GUI/BitCrusher.h"
#include "GUI/Clipper.h"
#include "GUI/WaveShaper.h"
#include "GUI/MasterControls.h"

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

private:
    
    DistorKAudioProcessor& audioProcessor;

    Laf lnf;

    juce::URL url{ "https://kwhaley5.gumroad.com/" };

    juce::HyperlinkButton gumroad{ "More Plugins", url };

    ToggleComp toggleComp{ audioProcessor.apvts };
    MasterComp masterComp{ audioProcessor };
    SaturationComp satComp{ audioProcessor.apvts };
    BitCrusherComp bitComp{ audioProcessor.apvts };
    ClipperComp clipperComp{ audioProcessor.apvts };
    WaveShaperComp wsComp{ audioProcessor.apvts };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistorKAudioProcessorEditor)
};
