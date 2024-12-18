/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"
#include "PluginProcessor.h"
#include "GUI/kLookAndFeel.h"
#include "GUI/rotarySliderWithLabels.h"
#include "GUI/Toggle.h"
#include "GUI/Saturation.h"
#include "GUI/BitCrusher.h"
#include "GUI/Clipper.h"
#include "GUI/WaveShaper.h"
#include "GUI/MasterControls.h"
#include "GUI/Toolbar.h"

//==============================================================================
/**
*/
class DistorKAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                     public juce::Timer
{
public:
    DistorKAudioProcessorEditor (DistorKAudioProcessor&);
    ~DistorKAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    
    void displayCorrectDistortion();

    DistorKAudioProcessor& audioProcessor;

    Laf lnf;

    juce::URL url{ "https://kwhaley5.gumroad.com/" };

    juce::HyperlinkButton gumroad{ "More Plugins", url };

    //ToggleComp toggleComp{ audioProcessor.apvts };
    ToolbarComp toolbar{ audioProcessor.apvts };
    MasterComp masterComp{ audioProcessor };
    SaturationComp satComp{ audioProcessor.apvts };
    BitCrusherComp bitComp{ audioProcessor.apvts };
    ClipperComp clipperComp{ audioProcessor.apvts };
    WaveShaperComp wsComp{ audioProcessor.apvts };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistorKAudioProcessorEditor)
};
