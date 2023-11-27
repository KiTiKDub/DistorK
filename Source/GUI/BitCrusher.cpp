/*
  ==============================================================================

    BitCrusher.cpp
    Created: 17 Nov 2023 4:40:15pm
    Author:  kylew

  ==============================================================================
*/

#include "BitCrusher.h"

BitCrusherComp::BitCrusherComp(juce::AudioProcessorValueTreeState& apvts)
{
    updateRSWL(apvts);

    addAndMakeVisible(*rate);
    addAndMakeVisible(*depth);
    addAndMakeVisible(*inGain);
    addAndMakeVisible(*mix);
    addAndMakeVisible(*outGain);
}

BitCrusherComp::~BitCrusherComp()
{

}

void BitCrusherComp::paint(juce::Graphics& g)
{

}

void BitCrusherComp::resized()
{
    auto bounds = getLocalBounds();

    auto bigKnob = bounds.removeFromTop(bounds.getHeight() * .8);
    auto bigRight = bigKnob.removeFromRight(bigKnob.getWidth() * .5);
    rate->setBounds(bigKnob);
    depth->setBounds(bigRight);

    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .5);

    inGain->setBounds(leftSide);
    outGain->setBounds(rightSide);
    mix->setBounds(bounds);
}

void BitCrusherComp::updateRSWL(juce::AudioProcessorValueTreeState& apvts)
{
    auto& inGainParam = getParam(apvts, "crusherInGain");
    auto& rateParam = getParam(apvts, "crusherBitRate");
    auto& depthParam = getParam(apvts, "crusherBitDepth");
    auto& mixParam = getParam(apvts, "crusherMix");
    auto& outGainParam = getParam(apvts, "crusherOutGain");

    inGain = std::make_unique<RotarySliderWithLabels>(&inGainParam, "dB", "In Gain");
    rate = std::make_unique<RotarySliderWithLabels>(&rateParam, "", "Bit Rate");
    depth = std::make_unique<RotarySliderWithLabels>(&depthParam, "", "Bit Depth");
    mix = std::make_unique<RotarySliderWithLabels>(&mixParam, "%", "Mix");
    outGain = std::make_unique<RotarySliderWithLabels>(&outGainParam, "dB", "Out Gain");

    makeAttachment(inGainAT, apvts, "crusherInGain", *inGain);
    makeAttachment(rateAT, apvts, "crusherBitRate", *rate);
    makeAttachment(depthAT, apvts, "crusherBitDepth", *depth);
    makeAttachment(mixAT, apvts, "crusherMix", *mix);
    makeAttachment(outGainAT, apvts, "crusherOutGain", *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB"); 
    addLabelPairs(rate->labels, 1, 3, rateParam, "", 20);
    addLabelPairs(depth->labels, 1, 3, depthParam, "", 20);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
        };
    rate.get()->onValueChange = [this, &rateParam]()
        {
            addLabelPairs(rate->labels, 1, 3, rateParam, "", 20);
        };
    depth.get()->onValueChange = [this, &depthParam]()
        {
            addLabelPairs(depth->labels, 1, 3, depthParam, "", 20);
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
