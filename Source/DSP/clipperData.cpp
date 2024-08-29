/*
  ==============================================================================

    clipper.cpp
    Created: 2 Nov 2023 1:42:41pm
    Author:  kylew

  ==============================================================================
*/

#include "ClipperData.h"

void Clipper::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
    //I guess I don't need these anymore...
}

void Clipper::process(juce::dsp::AudioBlock<float>& block)
{
    if (clipperBypass) { return; };

    for (int channel = 0; channel < block.getNumChannels(); channel++)
    {
        osg.process(block, clipperGainIn, channel);

        switch (clipperMode)
        {
        case Clipper::hardClipper:
            processHardClipper(channel, block);
            break;

        case Clipper::cubic:
            processCubic(channel, block);
            break;

        case Clipper::sin:
            processSin(channel, block);
            break;

        case Clipper::hTangent:
            processHyperTangent(channel, block);
            break;

        case Clipper::arcTangent:
            processArcTangent(channel, block);
            break;

        case Clipper::quintic:
            processQuintic(channel, block);
            break;
        }

        osg.process(block, clipperGainOut, channel);
    }

}

void Clipper::updateParams(bool bypass, int mode, float threshold, float gainIn, float gainOut, float mix)
{
    clipperBypass = bypass;
    clipperMode = mode;
    clipperThresh = threshold;
    clipperGainIn = gainIn;
    clipperGainOut = gainOut;
    clipperMix = mix/100;
}

void Clipper::processHardClipper(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        auto gain = juce::Decibels::decibelsToGain(clipperThresh);
        auto drySignal = data[s];
        data[s] > gain ? data[s] = (gain * clipperMix) + drySignal * (1 - clipperMix) : data[s] = data[s];
        data[s] < -gain ? data[s] = (-gain * clipperMix) + drySignal * (1 - clipperMix) : data[s] = data[s];
    }
}

void Clipper::processCubic(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); s++)
    {
        auto newLimit = juce::Decibels::decibelsToGain(clipperThresh);
        auto drySignal = data[s];
        auto inverse = 1 / newLimit;
        auto resizeSamples = data[s] * inverse;
        resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
        resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
        auto cubic = (resizeSamples - pow(resizeSamples, 3) / 3);
        data[s] = (cubic * newLimit * clipperMix) + (drySignal * (1 - clipperMix));
    }
}

void Clipper::processSin(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        auto newLimit = juce::Decibels::decibelsToGain(clipperThresh);
        auto drySignal = data[s];
        auto inverse = 1 / newLimit;
        auto resizeSamples = data[s] * inverse;
        resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
        resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
        auto sinosidal = std::sin(1 * juce::MathConstants<float>::pi * resizeSamples / 3);

        data[s] = (sinosidal * newLimit * clipperMix) + (drySignal * (1 - clipperMix));
    }
}

void Clipper::processHyperTangent(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); s++)
    {
        auto newLimit = juce::Decibels::decibelsToGain(clipperThresh);
        auto drySignal = data[s];
        auto inverse = 1 / newLimit;
        auto resizeSamples = data[s] * inverse;
        resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
        resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
        auto hyperTan = tanh(resizeSamples);
        data[s] = (hyperTan * newLimit * clipperMix) + (drySignal * (1 - clipperMix));
    }
}

void Clipper::processArcTangent(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        auto newLimit = juce::Decibels::decibelsToGain(clipperThresh);
        auto drySignal = data[s];
        auto inverse = 1 / newLimit;
        auto resizeSamples = data[s] * inverse;
        resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
        resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
        auto arcTan = atan(resizeSamples * juce::MathConstants<float>::pi / 2) * (2 / juce::MathConstants<float>::pi);
        data[s] = (arcTan * newLimit * clipperMix) + (drySignal * (1 - clipperMix));
    }
}

void Clipper::processQuintic(int channel, juce::dsp::AudioBlock<float>& block)
{
    auto data = block.getChannelPointer(channel);

    for (int s = 0; s < block.getNumSamples(); ++s)
    {
        auto newLimit = juce::Decibels::decibelsToGain(clipperThresh);
        auto drySignal = data[s];
        auto inverse = 1 / newLimit;
        auto resizeSamples = data[s] * inverse;
        resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
        resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
        auto quintic = resizeSamples - pow(resizeSamples, 5) / 5;
        data[s] = (quintic * newLimit * clipperMix) + (drySignal * (1 - clipperMix));
    }
}
