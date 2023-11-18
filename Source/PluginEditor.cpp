/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistorKAudioProcessorEditor::DistorKAudioProcessorEditor (DistorKAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    selectClipAT(audioProcessor.apvts, "selectClip", selectClip),
    selectBitAT(audioProcessor.apvts, "selectBit", selectBit),
    selectWaveShprAT(audioProcessor.apvts, "selectWaveShpr", selectWaveShpr),
    selectSatAT(audioProcessor.apvts, "selectSat", selectSat)
{
    setLookAndFeel(&lnf);

    attachRSWL();
    addAndMakeVisible(*oversampleSelect);

    addAndMakeVisible(selectClip);
    addAndMakeVisible(selectBit);
    addAndMakeVisible(selectWaveShpr);
    addAndMakeVisible(selectSat);

    selectClip.onClick = [this] { updateToggleState(&selectClip); };
    selectBit.onClick = [this] { updateToggleState(&selectBit); };
    selectWaveShpr.onClick = [this] { updateToggleState(&selectWaveShpr); };
    selectSat.onClick = [this] { updateToggleState(&selectSat); };

    selectClip.setRadioGroupId(1);
    selectBit.setRadioGroupId(1);
    selectWaveShpr.setRadioGroupId(1);
    selectSat.setRadioGroupId(1);
    

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

    auto t1Area = selectArea.removeFromLeft(bounds.getWidth() * .2);
    auto t2Area = selectArea.removeFromLeft(bounds.getWidth() * .25);
    auto osArea = selectArea.removeFromLeft(bounds.getWidth() * .33);
    auto t3Area = selectArea.removeFromLeft(bounds.getWidth() * .5);
    auto t4Area = selectArea;

    selectSat.setBounds(t1Area);
    selectClip.setBounds(t2Area);
    oversampleSelect.get()->setBounds(osArea);
    selectWaveShpr.setBounds(t3Area);
    selectBit.setBounds(t4Area);
}

void DistorKAudioProcessorEditor::attachRSWL()
{
    auto& osParam = getParam(audioProcessor.apvts, "overSampleSelect");
    oversampleSelect = std::make_unique<RotarySliderWithLabels>(&osParam, "dB", "OverSampling");
    makeAttachment(oversampleSelectAT, audioProcessor.apvts, "overSampleSelect", *oversampleSelect);
    addLabelPairs(oversampleSelect->labels, 1, 3, osParam, "", oversamplingText);

    oversampleSelect.get()->onValueChange = [this, &osParam]()
        {
            addLabelPairs(oversampleSelect->labels, 1, 3, osParam, "", oversamplingText);
        };
}

void DistorKAudioProcessorEditor::updateToggleState(juce::Button* button)
{
    auto state = button->getToggleState();
}
