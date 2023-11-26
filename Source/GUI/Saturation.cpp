/*
  ==============================================================================

    Saturation.cpp
    Created: 17 Nov 2023 4:41:53pm
    Author:  kylew

  ==============================================================================
*/

#include "Saturation.h"

#define NORMAL_FONT_SIZE 14

SaturationComp::SaturationComp(juce::AudioProcessorValueTreeState& apvts)
{
    updateRSWL(apvts);

    addAndMakeVisible(*inGain);
    addAndMakeVisible(*drive);
    addAndMakeVisible(*mix);
    addAndMakeVisible(*outGain);
}

SaturationComp::~SaturationComp()
{

}

void SaturationComp::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::white);

    auto bigKnob = bounds.removeFromTop(bounds.getHeight() * .7);

    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .5);

    /*g.drawRect(leftSide);
    g.drawRect(rightSide);
    g.drawRect(bounds);*/
}

void SaturationComp::resized()
{
    auto bounds = getLocalBounds();

    auto bigKnob = bounds.removeFromTop(bounds.getHeight() * .8);
    drive->setBounds(bigKnob);

    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .5);

    inGain->setBounds(leftSide);
    outGain->setBounds(rightSide);
    mix->setBounds(bounds);

}

void SaturationComp::updateRSWL(juce::AudioProcessorValueTreeState& apvts)
{
    auto& inGainParam = getParam(apvts, "satInGain");
    auto& driveParam = getParam(apvts, "satDrive");
    auto& mixParam = getParam(apvts, "satMix");
    auto& outGainParam = getParam(apvts, "satOutGain");

    inGain = std::make_unique<RotarySliderWithLabels>(&inGainParam, "dB", "In Gain");
    drive = std::make_unique<RotarySliderWithLabels>(&driveParam, "", "Drive");
    mix = std::make_unique<RotarySliderWithLabels>(&mixParam, "%", "Mix");
    outGain = std::make_unique<RotarySliderWithLabels>(&outGainParam, "dB", "Out Gain");

    makeAttachment(inGainAT, apvts, "satInGain", *inGain);
    makeAttachment(driveAT, apvts, "satDrive", *drive);
    makeAttachment(mixAT, apvts, "satMix", *mix);
    makeAttachment(outGainAT, apvts, "satOutGain", *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB", NORMAL_FONT_SIZE, empty);
    addLabelPairs(drive->labels, 1, 3, driveParam, "", 24, empty);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%", NORMAL_FONT_SIZE, empty);
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB", NORMAL_FONT_SIZE, empty);

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB", NORMAL_FONT_SIZE, empty);
        };
    drive.get()->onValueChange = [this, &driveParam]()
        {
            addLabelPairs(drive->labels, 1, 3, driveParam, "", 24, empty);
        };
    mix.get()->onValueChange = [this, &mixParam]()
        {
            addLabelPairs(mix->labels, 1, 3, mixParam, "%", NORMAL_FONT_SIZE, empty);
        };
    outGain.get()->onValueChange = [this, &outGainParam]()
        {
            addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB", NORMAL_FONT_SIZE, empty);
        };
}
