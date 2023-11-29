/*
  ==============================================================================

    Clipper.cpp
    Created: 17 Nov 2023 4:40:46pm
    Author:  kylew

  ==============================================================================
*/

#include "Clipper.h"

ClipperComp::ClipperComp(juce::AudioProcessorValueTreeState& apvts)
{
    updateRSWL(apvts);
    updateSWL(apvts);

    addAndMakeVisible(*select);
    addAndMakeVisible(*threshold);
    addAndMakeVisible(*inGain);
    addAndMakeVisible(*mix);
    addAndMakeVisible(*outGain);
}

ClipperComp::~ClipperComp()
{

}

void ClipperComp::paint(juce::Graphics& g)
{

}

void ClipperComp::resized()
{
    auto bounds = getLocalBounds();

    auto bigKnob = bounds.removeFromTop(bounds.getHeight() * .8);
    auto sliderSpace = bigKnob.removeFromRight(bigKnob.getWidth() * .2);
    sliderSpace.reduce(0, sliderSpace.getHeight() * .1);
    sliderSpace.removeFromBottom(sliderSpace.getHeight() * .05);
    threshold->setBounds(sliderSpace);
    select->setBounds(bigKnob);

    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .5);

    inGain->setBounds(leftSide);
    outGain->setBounds(rightSide);
    mix->setBounds(bounds);
}

void ClipperComp::updateRSWL(juce::AudioProcessorValueTreeState& apvts)
{
    auto& inGainParam = getParam(apvts, "clipperInGain");
    auto& selectParam = getParam(apvts, "clipperSelect");
    auto& mixParam = getParam(apvts, "clipperMix");
    auto& outGainParam = getParam(apvts, "clipperOutGain");

    inGain = std::make_unique<RotarySliderWithLabels>(&inGainParam, "dB", "In Gain");
    select = std::make_unique<RotarySliderWithLabels>(&selectParam, "", "Select");
    mix = std::make_unique<RotarySliderWithLabels>(&mixParam, "%", "Mix");
    outGain = std::make_unique<RotarySliderWithLabels>(&outGainParam, "dB", "Out Gain");

    makeAttachment(inGainAT, apvts, "clipperInGain", *inGain);
    makeAttachment(selectAT, apvts, "clipperSelect", *select);
    makeAttachment(mixAT, apvts, "clipperMix", *mix);
    makeAttachment(outGainAT, apvts, "clipperOutGain", *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
    addLabelPairs(select->labels, 1, 3, selectParam, "", 24, typeText);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
        };
    select.get()->onValueChange = [this, &selectParam]()
        {
            addLabelPairs(select->labels, 1, 3, selectParam, "", 24, typeText);
        };
    mix.get()->onValueChange = [this, &mixParam]()
        {
            addLabelPairs(mix->labels, 1, 3, mixParam, "%");
        };
    outGain.get()->onValueChange = [this, &outGainParam]()
        {
            addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");
        };
}

void ClipperComp::updateSWL(juce::AudioProcessorValueTreeState& apvts)
{
    auto& thresholdParm = getSliderParam(apvts, "clipperThresh");
    threshold = std::make_unique<SliderWithLabels>(&thresholdParm, "Threshold", juce::Slider::LinearVertical);
    makeSliderAttachment(thresholdAT, apvts, "clipperThresh", *threshold);
    addLabelPairs(threshold->labels, thresholdParm, " dB");

    threshold.get()->onValueChange = [this, &thresholdParm]()
        {
            addLabelPairs(threshold->labels, thresholdParm, " dB");
        };
}
