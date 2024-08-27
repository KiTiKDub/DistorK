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

void Clipper::process(juce::dsp::ProcessContextReplacing<float>& context, int ovRate, std::array<juce::dsp::Oversampling<float>, 4>& overSamplers)
{
    if (clipperBypass) { return; };

    inGain.setGainDecibels(clipperGainIn);
    inGain.process(context);
    
    auto ovBlock = overSamplers[ovRate].processSamplesUp(context.getInputBlock());
    auto len = ovBlock.getNumSamples();


    for (int channel = 0; channel < ovBlock.getNumChannels(); channel++)
    {
        auto data = ovBlock.getChannelPointer(channel);

        switch (clipperMode)
        {
        case Clipper::hardClipper:
            for (int s = 0; s < len; ++s)
            {
                auto gain = juce::Decibels::decibelsToGain(clipperThresh);
                auto drySignal = data[s];
                data[s] > gain ? data[s] = (gain * clipperMix) + drySignal * (1 - clipperMix) : data[s] = data[s];
                data[s] < -gain ? data[s] = (-gain * clipperMix) + drySignal * (1 - clipperMix) : data[s] = data[s];
            }
            break;

        case Clipper::cubic:
            for (int s = 0; s < len; s++)
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
            break;

        case Clipper::sin:
            for (int s = 0; s < len; ++s)
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
            break;

        case Clipper::hTangent:
            for (int s = 0; s < len; s++)
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
            break;

        case Clipper::arcTangent:
            for (int s = 0; s < len; ++s)
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
            break;

        case Clipper::quintic:
            for (int s = 0; s < len; ++s)
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
            break;
        }
    }

    overSamplers[ovRate].processSamplesDown(context.getOutputBlock());

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
