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

void DistorkEngine::process(juce::dsp::ProcessContextReplacing<float>& context, std::vector<int> distortionProcessOrder, int ovRate, std::array<juce::dsp::Oversampling<float>, 4>& overSamplers)
{
    if (engineToggle->get()) { return; }

    updateParams();

    for (int i = 0; i < distortionProcessOrder.size(); i++)
    {
        if (distortionProcessOrder[i] == 1)
            saturator.process(context, ovRate, overSamplers);
        else if (distortionProcessOrder[i] == 2)
            clipper.process(context, ovRate, overSamplers);
        else if (distortionProcessOrder[i] == 3)
            waveshaper.process(context, ovRate, overSamplers);
        else if (distortionProcessOrder[i] == 4)
            bitcrusher.process(context, ovRate, overSamplers);
    }
}

void DistorkEngine::updateParams()
{
    saturator.updateParams(satToggle->get(), satDrive->get(), satInGain->get(), satOutGain->get(), satMix->get());
    clipper.updateParams(clipperToggle->get(), clipperSelect->get(), clipperThresh->get(), clipperInGain->get(), clipperOutGain->get(), clipperMix->get());
    waveshaper.updateParams(waveShaperToggle->get(), waveShaperSelect->get(), waveShaperFactorsHolder, waveShaperInGain->get(), waveShaperOutGain->get(), waveShaperMix->get());
    bitcrusher.updateParams(crusherToggle->get(), crusherBitDepth->get(), crusherBitRate->get(), crusherInGain->get(), crusherOutGain->get(), crusherMix->get());
}
