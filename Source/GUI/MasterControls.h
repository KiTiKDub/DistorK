/*
  ==============================================================================

    MasterControls.h
    Created: 17 Nov 2023 4:39:37pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include "BinaryData.h"
#include "../PluginProcessor.h"
#include "kLevelMeter.h"
#include "SliderWithLabels.h"

struct GainCompensator : public juce::Component
{
   GainCompensator(DistorKAudioProcessor &);
   ~GainCompensator() {};

  void paint(juce::Graphics &g) override;
  void resized() override {};
  float getCompensateValue();

private: 
  DistorKAudioProcessor &audioP;
  juce::SmoothedValue<float> smoothValue;
};

struct MasterComp : public juce::Component
{
    MasterComp(DistorKAudioProcessor&);
    ~MasterComp();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void update();

private:

    DistorKAudioProcessor& audioP;

    juce::Image logo;
    juce::Font newFont;

    void updateSWL(juce::AudioProcessorValueTreeState& apvts);

    std::array<LevelMeter, 2> meter;
    std::array<LevelMeter, 2> outMeter;

    std::unique_ptr<SliderWithLabels> inGain, mix, outGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainAT, mixAT, outGainAT;

    juce::Slider testSlider;

    juce::ToggleButton bypass{ "Bypass" };
    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAT;

    GainCompensator gainCompensator;

    juce::Time timer;
};