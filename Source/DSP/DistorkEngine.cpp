/*
  ==============================================================================

    DistorkEngine.cpp
    Created: 26 Jan 2024 7:02:01am
    Author:  kylew

  ==============================================================================
*/

#include "DistorkEngine.h"

void DistorkEngine::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
    saturator.prepareToPlay(spec);
    clipper.prepareToPlay(spec);
    waveshaper.prepareToPlay(spec);
    bitcrusher.prepareToPlay(spec);
}

void DistorkEngine::process(juce::dsp::AudioBlock<float>& block, std::vector<int> order, int channel)
{
    if (engineToggle->get()) { return; }

    updateParams();

    for (int i = 0; i < order.size(); i++)
    {
        if (order[i] == 1)
            saturator.process(block, channel);
        else if (order[i] == 2)
            clipper.process(block, channel);
        else if (order[i] == 3)
            waveshaper.process(block, channel);
        else if (order[i] == 4)
            bitcrusher.process(block, channel);
    }
}

void DistorkEngine::updateParams()
{
    saturator.updateParams(satToggle->get(), satDrive->get(), satInGain->get(), satOutGain->get(), satMix->get());
    clipper.updateParams(clipperToggle->get(), clipperSelect->get(), clipperThresh->get(), clipperInGain->get(), clipperOutGain->get(), clipperMix->get());
    waveshaper.updateParams(waveShaperToggle->get(), waveShaperSelect->get(), waveShaperFactorsHolder, waveShaperInGain->get(), waveShaperOutGain->get(), waveShaperMix->get());
    bitcrusher.updateParams(crusherToggle->get(), crusherBitDepth->get(), crusherBitRate->get(), crusherInGain->get(), crusherOutGain->get(), crusherMix->get());
}
