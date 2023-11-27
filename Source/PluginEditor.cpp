/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistorKAudioProcessorEditor::DistorKAudioProcessorEditor (DistorKAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&lnf);

    addAndMakeVisible(toggleComp);
    //addAndMakeVisible(satComp);
    //addAndMakeVisible(bitComp);
    addAndMakeVisible(clipperComp);

    setSize (700, 500);
}

DistorKAudioProcessorEditor::~DistorKAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void DistorKAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    auto bounds = getLocalBounds();

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);

    auto selectArea = bounds.removeFromBottom(bounds.getHeight() * .15);
    auto masterArea = bounds.removeFromRight(bounds.getWidth() * .3);
    g.drawRect(selectArea);
    g.drawRect(masterArea);

    //g.drawFittedText("Select Area", selectArea.toNearestInt(), juce::Justification::centred, 1);
    g.drawFittedText("Master Area", masterArea.toNearestInt(), juce::Justification::centred, 1);
    g.drawFittedText("DistorK", bounds.toNearestInt(), juce::Justification::centred, 1);

}

void DistorKAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto selectArea = bounds.removeFromBottom(bounds.getHeight() * .15);
    auto masterArea = bounds.removeFromRight(bounds.getWidth() * .3);

    toggleComp.setBounds(selectArea);

    //satComp.setBounds(bounds);
    //bitComp.setBounds(bounds);
    clipperComp.setBounds(bounds);
}




