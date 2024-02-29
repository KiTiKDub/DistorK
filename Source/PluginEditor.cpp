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

    addAndMakeVisible(toolbar);
    addAndMakeVisible(masterComp);
    addAndMakeVisible(satComp);
    addChildComponent(bitComp);
    addChildComponent(clipperComp);
    addChildComponent(wsComp);
    addAndMakeVisible(gumroad);
    
    setSize (700, 500);
    startTimerHz(24);
}

DistorKAudioProcessorEditor::~DistorKAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void DistorKAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::white);
    
    auto logoSpace = bounds.reduced(bounds.getWidth() * .425, bounds.getHeight() * .425);
    logoSpace.setX(bounds.getTopLeft().getX());
    logoSpace.setY(bounds.getTopLeft().getY());
    logoSpace.translate(-25, -10);

    auto logo = juce::ImageCache::getFromMemory(BinaryData::KITIK_LOGO_NO_BKGD_png, BinaryData::KITIK_LOGO_NO_BKGD_pngSize);
    g.setOpacity(.8);
    g.drawImage(logo, logoSpace.toFloat(), juce::RectanglePlacement::centred);
    g.setOpacity(1);

    g.setFont(15.0f);

    auto selectArea = bounds.removeFromBottom(bounds.getHeight() * .15);
    auto masterArea = bounds.removeFromRight(bounds.getWidth() * .3);

    g.setColour(juce::Colour(186u, 34u, 34u));
    juce::Line<int> line;

    line.setStart(selectArea.getTopLeft());
    line.setEnd(selectArea.getTopRight());
    g.drawLine(line.toFloat(), 1.f);

    line.setStart(masterArea.getTopLeft());
    line.setEnd(masterArea.getBottomLeft());
    g.drawLine(line.toFloat(), 1.f);
}

void DistorKAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto selectArea = bounds.removeFromBottom(bounds.getHeight() * .15);
    auto masterArea = bounds.removeFromRight(bounds.getWidth() * .3);

    toolbar.setBounds(selectArea);
    masterComp.setBounds(masterArea);

    satComp.setBounds(bounds);
    bitComp.setBounds(bounds);
    clipperComp.setBounds(bounds);
    wsComp.setBounds(bounds);

    auto linkSpace = bounds.reduced(bounds.getWidth() * .4, bounds.getHeight() * .45);
    linkSpace.setX(bounds.getTopRight().getX() - linkSpace.getWidth());
    linkSpace.setY(bounds.getTopRight().getY());

    auto font = juce::Font::Font();
    gumroad.setFont(font, false);
    gumroad.setColour(0x1001f00, juce::Colours::white);
    gumroad.setBounds(linkSpace);
}

void DistorKAudioProcessorEditor::timerCallback()
{
    if (toolbar.isMouseOver(true)) 
        displayCorrectDistortion();

    masterComp.update();
    audioProcessor.setToolbarOrder(toolbar.getToolbar()->getAllItems());
}

void DistorKAudioProcessorEditor::displayCorrectDistortion()
{
    auto currentToolbar = toolbar.getToolbar();

    auto& childComps = currentToolbar->getChildren();
    for (auto* child : childComps)
    {
        if (auto* tbComp = dynamic_cast<KitikToolbarItemComponent*>(child))
        {
            if (tbComp->isClicked)
            {
                auto name = tbComp->getName();

                if (name == "Saturation")
                {
                    satComp.setVisible(true);
                    clipperComp.setVisible(false);
                    wsComp.setVisible(false);
                    bitComp.setVisible(false);
                }
                else if (name == "Clipper")
                {
                    satComp.setVisible(false);
                    clipperComp.setVisible(true);
                    wsComp.setVisible(false);
                    bitComp.setVisible(false);

                }
                else if (name == "Waveshaper")
                {
                    satComp.setVisible(false);
                    clipperComp.setVisible(false);
                    wsComp.setVisible(true);
                    bitComp.setVisible(false);
                }
                else if (name == "Bitcrusher")
                {
                    satComp.setVisible(false);
                    clipperComp.setVisible(false);
                    wsComp.setVisible(false);
                    bitComp.setVisible(true);
                }

            }
        }
    }
}




