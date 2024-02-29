/*
  ==============================================================================

    WaveShaper.cpp
    Created: 17 Nov 2023 4:40:32pm
    Author:  kylew

  ==============================================================================
*/

#include "WaveShaper.h"

WaveShaperComp::WaveShaperComp(juce::AudioProcessorValueTreeState& apvts)
{
    updateRSWL(apvts);

    addAndMakeVisible(*select);
    addAndMakeVisible(*distort);
    addAndMakeVisible(*inGain);
    addAndMakeVisible(*mix);
    addAndMakeVisible(*outGain);
 
    updateAttachments(apvts);
}

WaveShaperComp::~WaveShaperComp()
{

}

void WaveShaperComp::paint(juce::Graphics& g)
{

}

void WaveShaperComp::resized()
{
    auto bounds = getLocalBounds();

    auto bigKnob = bounds.removeFromTop(bounds.getHeight() * .8);
    auto bigRight = bigKnob.removeFromRight(bigKnob.getWidth() * .5);
    select->setBounds(bigKnob);
    distort->setBounds(bigRight);

    auto leftSide = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto rightSide = bounds.removeFromRight(bounds.getWidth() * .5);

    inGain->setBounds(leftSide);
    outGain->setBounds(rightSide);
    mix->setBounds(bounds);
}

void WaveShaperComp::updateRSWL(juce::AudioProcessorValueTreeState& apvts)
{
    auto& inGainParam = getParam(apvts, "waveShaperInGain");
    auto& selectParam = getParam(apvts, "waveShaperSelect");
    auto& distortParm = getParam(apvts, "waveShaperSin");
    auto& mixParam = getParam(apvts, "waveShaperMix");
    auto& outGainParam = getParam(apvts, "waveShaperOutGain");

    inGain = std::make_unique<RotarySliderWithLabels>(&inGainParam, "dB", "In Gain");
    select = std::make_unique<RotarySliderWithLabels>(&selectParam, "", "Select");
    distort = std::make_unique<RotarySliderWithLabels>(&distortParm, "", "Distortion Factor");
    mix = std::make_unique<RotarySliderWithLabels>(&mixParam, "%", "Mix");
    outGain = std::make_unique<RotarySliderWithLabels>(&outGainParam, "dB", "Out Gain");

    makeAttachment(inGainAT, apvts, "waveShaperInGain", *inGain);
    makeAttachment(selectAT, apvts, "waveShaperSelect", *select);
    makeAttachment(distortAT, apvts, "waveShaperSin", *distort);
    makeAttachment(mixAT, apvts, "waveShaperMix", *mix);
    makeAttachment(outGainAT, apvts, "waveShaperOutGain", *outGain);

    addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
    addLabelPairs(select->labels, 1, 3, selectParam, "", 20, typeText);
    addLabelPairs(distort->labels, 1, 3, distortParm, "", 20);
    addLabelPairs(mix->labels, 1, 3, mixParam, "%");
    addLabelPairs(outGain->labels, 1, 3, outGainParam, " dB");

    inGain.get()->onValueChange = [this, &inGainParam]()
        {
            addLabelPairs(inGain->labels, 1, 3, inGainParam, " dB");
        };
    select.get()->onValueChange = [this, &selectParam, &apvts]()
        {
            addLabelPairs(select->labels, 1, 3, selectParam, "", 20, typeText);
            updateAttachments(apvts);
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

void WaveShaperComp::updateAttachments(juce::AudioProcessorValueTreeState& apvts)
{
    distortAT.reset();
    juce::String newID;
    auto param = select.get()->getValue();

    if (param == 0)
    {
        newID = "waveShaperSin";
    }
    else if (param == 1)
    {
        newID = "waveShaperQuadratic";
    }
    else if (param == 2)
    {
        newID = "waveShaperFactor";
    }
    else
    {
        newID = "waveShaperGB";
    }

    
    auto& distortParm = getParam(apvts, newID);
    distort.get()->changeParam(&distortParm);
    makeAttachment(distortAT, apvts, newID, *distort);
    addLabelPairs(distort->labels, 1, 3, distortParm, "", 20);

    distort.get()->onValueChange = [this, &distortParm]()
        {
            addLabelPairs(distort->labels, 1, 3, distortParm, "", 20);
        };
    
}
