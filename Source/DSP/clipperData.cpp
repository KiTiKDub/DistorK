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
    inGain.reset();
    inGain.prepare(spec);
    inGain.setRampDurationSeconds(.05);

    outGain.reset();
    outGain.prepare(spec);
    outGain.setRampDurationSeconds(.05);
}

void Clipper::process(juce::dsp::AudioBlock<float>& block, int channel)
{
    if (clipperBypass) { return; };

    auto context = juce::dsp::ProcessContextReplacing<float>(block);

    inGain.setGainDecibels(clipperGainIn);
    inGain.process(context);

    auto* channelInput = context.getInputBlock().getChannelPointer(channel);
    auto* channelOutput = context.getOutputBlock().getChannelPointer(channel);

    auto len = context.getInputBlock().getNumSamples();


    switch (clipperMode)
    {
    case Clipper::hardClipper:
        for (int s = 0; s < len; ++s)
        {
            auto gain = juce::Decibels::decibelsToGain(clipperThresh);
            auto drySignal = channelInput[s];
            channelInput[s] > gain ? channelOutput[s] = (gain * clipperMix) + drySignal * (1 - clipperMix) : channelOutput[s] = channelInput[s];
            channelInput[s] < -gain ? channelOutput[s] = (-gain * clipperMix) + drySignal * (1 - clipperMix) : channelOutput[s] = channelInput[s];
        }
        break;

    case Clipper::cubic:
        for (int s = 0; s < len; s++)
        {

            auto newLimit = juce::Decibels::decibelsToGain(clipperThresh);
            auto drySignal = channelInput[s];
            auto inverse = 1 / newLimit;
            auto resizeSamples = channelInput[s] * inverse;
            resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
            resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
            auto cubic = (resizeSamples - pow(resizeSamples, 3) / 3);
            channelOutput[s] = (cubic * newLimit * clipperMix) + (drySignal * (1 - clipperMix));
        }
        break;

    case Clipper::sin:
        for (int s = 0; s < len; ++s)
        {
            auto newLimit = juce::Decibels::decibelsToGain(clipperThresh);
            auto drySignal = channelInput[s];
            auto inverse = 1 / newLimit;
            auto resizeSamples = channelInput[s] * inverse;
            resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
            resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
            auto sinosidal = std::sin(1 * juce::MathConstants<float>::pi * resizeSamples / 3);

            channelOutput[s] = (sinosidal * newLimit * clipperMix) + (drySignal * (1 - clipperMix));
        }
        break;

    case Clipper::hTangent:
        for (int s = 0; s < len; s++)
        {
            auto newLimit = juce::Decibels::decibelsToGain(clipperThresh);
            auto drySignal = channelInput[s];
            auto inverse = 1 / newLimit;
            auto resizeSamples = channelInput[s] * inverse;
            resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
            resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
            auto hyperTan = tanh(resizeSamples);
            channelOutput[s] = (hyperTan * newLimit * clipperMix) + (drySignal * (1 - clipperMix));
        }
        break;

    case Clipper::arcTangent:
        for (int s = 0; s < len; ++s)
        {
            auto newLimit = juce::Decibels::decibelsToGain(clipperThresh);
            auto drySignal = channelInput[s];
            auto inverse = 1 / newLimit;
            auto resizeSamples = channelInput[s] * inverse;
            resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
            resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
            auto arcTan = atan(resizeSamples * juce::MathConstants<float>::pi /2) * (2/ juce::MathConstants<float>::pi);
            channelOutput[s] = (arcTan * newLimit * clipperMix) + (drySignal * (1 - clipperMix));
        }
        break;

    case Clipper::quintic:
        for (int s = 0; s < len; ++s)
        {
            auto newLimit = juce::Decibels::decibelsToGain(clipperThresh);
            auto drySignal = channelInput[s];
            auto inverse = 1 / newLimit;
            auto resizeSamples = channelInput[s] * inverse;
            resizeSamples > 1 ? resizeSamples = 1 : resizeSamples = resizeSamples;
            resizeSamples < -1 ? resizeSamples = -1 : resizeSamples = resizeSamples;
            auto quintic = resizeSamples - pow(resizeSamples, 5) / 5;
            channelOutput[s] = (quintic * newLimit * clipperMix) + (drySignal * (1 - clipperMix));
        }
        break;
    }


    outGain.setGainDecibels(clipperGainOut);
    outGain.process(context);

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
