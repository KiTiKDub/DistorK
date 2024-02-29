/*
  ==============================================================================

    Toolbar.cpp
    Created: 22 Jan 2024 10:04:29am
    Author:  kylew

  ==============================================================================
*/

#include "Toolbar.h"

ToolbarComp::ToolbarComp(AudioProcessorValueTreeState& apvts)
{
    setLookAndFeel(&lnf);

    attachRSWL(apvts);

    addAndMakeVisible(toolbar);
    toolbar.addDefaultItems(tbf);
    toolbar.setComponentID("toolbar");
    toolbar.setEditingActive(true);
    toolbar.getItemComponent(0)->isClicked = true;

    addAndMakeVisible(*oversampleSelect);

    setPowerButtons(apvts);
}

ToolbarComp::~ToolbarComp()
{
    setLookAndFeel(nullptr);
}

void ToolbarComp::resized()
{
    auto bounds = getLocalBounds();
    auto barSelect = bounds.removeFromLeft(bounds.getWidth() * .1);
    auto overSelect = bounds.removeFromRight(bounds.getWidth() * .11);
    overSelect.translate(0, 5);

    auto topSelect = barSelect.removeFromTop(barSelect.getHeight() * .33);
    auto midSelect = barSelect.removeFromTop(barSelect.getHeight() * .5);

    oversampleSelect->setBounds(overSelect);
}

void ToolbarComp::setPowerButtons(juce::AudioProcessorValueTreeState& apvts)
{
    auto children = toolbar.getChildren();

    //Needs to remove "Additional Items" from the array
    children.removeLast(1);

    std::vector<juce::String> Names
    {
        "selectSat",
        "selectClip",
        "selectBit",
        "selectWaveShpr"
    };

    for (int i = 0; i < children.size(); i++) //maybe change this so it can dynmically update the power buttons
    {
        if (auto* tbComp = dynamic_cast<KitikToolbarItemComponent*>(children[i]))
        {
            if(auto* power = dynamic_cast<juce::ToggleButton*>(tbComp->getChildComponent(0)))
            {
                //auto check = Names.at(i);
                auto ID = apvts.getParameter(Names.at(i))->getParameterID();
                auto attachment = vectorAT.at(i);
                *attachment = std::make_unique<Attachment>(apvts, ID, *power);
            }
            
        }
    }
}

void ToolbarComp::attachRSWL(juce::AudioProcessorValueTreeState& apvts)
{
    auto& osParam = getParam(apvts, "overSampleSelect");
    oversampleSelect = std::make_unique<RotarySliderWithLabels>(&osParam, "", "OverSampling");
    makeAttachment(oversampleSelectAT, apvts, "overSampleSelect", *oversampleSelect);
    addLabelPairs(oversampleSelect->labels, 1, 3, osParam, "", 14, oversamplingText);

    oversampleSelect.get()->onValueChange = [this, &osParam]()
        {
            addLabelPairs(oversampleSelect->labels, 1, 3, osParam, "", 14, oversamplingText);
        };
}
