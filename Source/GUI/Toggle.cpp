/*
  ==============================================================================

    Toggle.cpp
    Created: 17 Nov 2023 4:41:18pm
    Author:  kylew

  ==============================================================================
*/

#include "Toggle.h"
#define NORMAL_FONT_SIZE 14

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

    attachRSWL(apvts);
    addAndMakeVisible(*oversampleSelect);

    addAndMakeVisible(selectClip);
    selectClip.setComponentID("Select");
    addAndMakeVisible(selectBit);
    selectBit.setComponentID("Select");
    addAndMakeVisible(selectWaveShpr);
    selectWaveShpr.setComponentID("Select");
    addAndMakeVisible(selectSat);
    selectSat.setComponentID("Select");

    addAndMakeVisible(toggleClip);
    toggleClip.setComponentID("Power");
    addAndMakeVisible(toggleBit);
    toggleBit.setComponentID("Power");
    addAndMakeVisible(toggleWaveShpr);
    toggleWaveShpr.setComponentID("Power");
    addAndMakeVisible(toggleSat);
    toggleSat.setComponentID("Power");

    comboSat.addItemList(strings, 0);
    comboBit.addItemList(strings, 0);
    comboClip.addItemList(strings, 0);
    comboWaveShpr.addItemList(strings, 1);

    comboSat.onChange = [this] {updateArrayOrder(comboSat.getComponentID(), comboSat.getSelectedItemIndex());};
    comboBit.onChange = [this] { updateArrayOrder(comboBit.getComponentID(), comboBit.getSelectedItemIndex()); };
    comboClip.onChange = [this] { updateArrayOrder(comboClip.getComponentID(), comboClip.getSelectedItemIndex()); };
    comboWaveShpr.onChange = [this] { updateArrayOrder(comboWaveShpr.getComponentID(), comboWaveShpr.getSelectedItemIndex()); };

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

}

void ToggleComp::resized()
{
    auto bounds = getLocalBounds();
    bounds.translate(0, 3);

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
    addLabelPairs(oversampleSelect->labels, 1, 3, osParam, "", NORMAL_FONT_SIZE, oversamplingText);

    oversampleSelect.get()->onValueChange = [this, &osParam]()
        {
            addLabelPairs(oversampleSelect->labels, 1, 3, osParam, "", NORMAL_FONT_SIZE, oversamplingText);
        };
}

void ToggleComp::updateToggleState(juce::Button* button)
{
    auto state = button->getToggleState();
}

void ToggleComp::updateArrayOrder(juce::String distType, int selectedValue)
{
    if(distType == "Sat")
    {
        int temp = order[0];
        order[0] = order[selectedValue - 1];
        order[selectedValue - 1] = temp;
    }
    else if (distType == "Clip")
    {
        int temp = order[1];
        order[1] = order[selectedValue - 1];
        order[selectedValue - 1] = temp;
    }
    if (distType == "Wave")
    {
        int temp = order[2];
        order[2] = order[selectedValue - 1];
        order[selectedValue - 1] = temp;
    }
    if (distType == "Bit")
    {
        int temp = order[3];
        order[3] = order[selectedValue - 1];
        order[selectedValue - 1] = temp;
    }
}
