/*
  ==============================================================================

    Toggle.cpp
    Created: 17 Nov 2023 4:41:18pm
    Author:  kylew

  ==============================================================================
*/

#include "Toggle.h"

ToggleComp::ToggleComp(juce::AudioProcessorValueTreeState& apvts) :
selectClipAT(apvts, "selectClip", selectClip),
selectBitAT(apvts, "selectBit", selectBit),
selectWaveShprAT(apvts, "selectWaveShpr", selectWaveShpr),
selectSatAT(apvts, "selectSat", selectSat),
toggleClipAT(apvts, "bypassClip", toggleClip),
toggleBitAT(apvts, "bypassBit", toggleBit),
toggleWaveShprAT(apvts, "bypassWaveShpr", toggleWaveShpr),
toggleSatAT(apvts, "bypassSat", toggleSat)

{
    //setLookAndFeel(&lnf);

    attachRSWL(apvts);
    addAndMakeVisible(*oversampleSelect);

    addAndMakeVisible(selectClip);
    addAndMakeVisible(selectBit);
    addAndMakeVisible(selectWaveShpr);
    addAndMakeVisible(selectSat);

    addAndMakeVisible(toggleClip);
    toggleClip.setComponentID("Power");
    addAndMakeVisible(toggleBit);
    toggleBit.setComponentID("Power");
    addAndMakeVisible(toggleWaveShpr);
    toggleWaveShpr.setComponentID("Power");
    addAndMakeVisible(toggleSat);
    toggleSat.setComponentID("Power");

    selectClip.onClick = [this] { updateToggleState(&selectClip); };
    selectBit.onClick = [this] { updateToggleState(&selectBit); };
    selectWaveShpr.onClick = [this] { updateToggleState(&selectWaveShpr); };
    selectSat.onClick = [this] { updateToggleState(&selectSat); };

    selectClip.setRadioGroupId(1);
    selectBit.setRadioGroupId(1);
    selectWaveShpr.setRadioGroupId(1);
    selectSat.setRadioGroupId(1);
}

ToggleComp::~ToggleComp()
{

}

void ToggleComp::paint(juce::Graphics& g)
{
    //g.fillAll(juce::Colours::black);

    //auto bounds = getLocalBounds();

    //g.setColour(juce::Colours::white);
    //g.setFont(15.0f);

    //auto selectArea = bounds.removeFromBottom(bounds.getHeight() * .15);
    //auto masterArea = bounds.removeFromRight(bounds.getWidth() * .3);
    //g.drawRect(selectArea);
    //g.drawRect(masterArea);

    ////g.drawFittedText("Select Area", selectArea.toNearestInt(), juce::Justification::centred, 1);
    //g.drawFittedText("Master Area", masterArea.toNearestInt(), juce::Justification::centred, 1);
    //g.drawFittedText("DistorK", bounds.toNearestInt(), juce::Justification::centred, 1);
}

void ToggleComp::resized()
{
    auto bounds = getLocalBounds();

    auto t1Area = bounds.removeFromLeft(bounds.getWidth() * .2);
    auto t1toggle = t1Area.reduced(t1Area.getWidth() * .3, t1Area.getHeight() * .3);
    t1toggle.setPosition(t1Area.getPosition());
    auto t2Area = bounds.removeFromLeft(bounds.getWidth() * .25);
    auto t2toggle = t2Area.reduced(t2Area.getWidth() * .3, t2Area.getHeight() * .3);
    t2toggle.setPosition(t2Area.getPosition());
    auto osArea = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto t3Area = bounds.removeFromLeft(bounds.getWidth() * .5);
    auto t3toggle = t3Area.reduced(t3Area.getWidth() * .3, t3Area.getHeight() * .3);
    t3toggle.setPosition(t3Area.getPosition());
    auto t4Area = bounds.removeFromLeft(bounds.getWidth());
    auto t4toggle = t4Area.reduced(t4Area.getWidth() * .3, t4Area.getHeight() * .3);
    t4toggle.setPosition(t4Area.getPosition());

    selectSat.setBounds(t1Area);
    toggleSat.setBounds(t1toggle);
    selectClip.setBounds(t2Area);
    toggleClip.setBounds(t2toggle);
    oversampleSelect.get()->setBounds(osArea);
    selectWaveShpr.setBounds(t3Area);
    toggleWaveShpr.setBounds(t3toggle);
    selectBit.setBounds(t4Area);
    toggleBit.setBounds(t4toggle);
}

void ToggleComp::attachRSWL(juce::AudioProcessorValueTreeState& apvts)
{
    auto& osParam = getParam(apvts, "overSampleSelect");
    oversampleSelect = std::make_unique<RotarySliderWithLabels>(&osParam, "", "OverSampling");
    makeAttachment(oversampleSelectAT, apvts, "overSampleSelect", *oversampleSelect);
    addLabelPairs(oversampleSelect->labels, 1, 3, osParam, "", oversamplingText);

    oversampleSelect.get()->onValueChange = [this, &osParam]()
        {
            addLabelPairs(oversampleSelect->labels, 1, 3, osParam, "", oversamplingText);
        };
}

void ToggleComp::updateToggleState(juce::Button* button)
{
    auto state = button->getToggleState();
}
