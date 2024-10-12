/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/kLevelMeterData.h"
#include "DSP/clipperData.h"
#include "DSP/BitCrusherData.h"
#include "DSP/SaturationData.h"
#include "DSP/WaveShaperData.h"
#include "DSP/DistorkEngine.h"

//==============================================================================
/**
*/
class DistorKAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    DistorKAudioProcessor();
    ~DistorKAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "parameters", createParameterLayout() };

    void setToolbarOrder(std::vector<int> tbOrder) { distortionProcessOrder = tbOrder; }

    LevelMeterData levelMeterData;

private:

    /*Clipper clipper;
    BitCrusher bitcrusher;
    WaveShaper waveshaper;
    Saturation saturation;*/

    DistorkEngine distorkEngine;

    std::array<juce::dsp::Oversampling<float>, 4> overSamplers
    { {
        {2, 0, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true},
        {2, 3, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true}
    } };

    int lastOSValue{ 0 };

    //Master DSP & Params
    juce::dsp::Gain<float> masterIn;
    juce::dsp::Gain<float> masterOut;
    
    juce::AudioParameterBool* globalBypass{ nullptr };
    juce::AudioParameterBool* selectClip{ nullptr };
    juce::AudioParameterBool* bypassClip{ nullptr };
    juce::AudioParameterBool* selectBit{ nullptr };
    juce::AudioParameterBool* bypassBit{ nullptr };
    juce::AudioParameterBool* selectWaveShpr{ nullptr };
    juce::AudioParameterBool* bypassWaveShpr{ nullptr };
    juce::AudioParameterBool* selectSat{ nullptr };
    juce::AudioParameterBool* bypassSat{ nullptr };
    juce::AudioParameterFloat* masterInValue{ nullptr };
    juce::AudioParameterFloat* masterOutValue{ nullptr };
    juce::AudioParameterFloat* masterMix{ nullptr };
    juce::AudioParameterInt* overSampleSelect{ nullptr };

    std::vector<int> distortionProcessOrder{ 1, 2, 3, 4 };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistorKAudioProcessor)
};
