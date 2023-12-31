/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DSP/kLevelMeterData.h"
#include "DSP/clipperData.h"
#include "DSP/BitCrusherData.h"
#include "DSP/SaturationData.h"
#include "DSP/WaveShaperData.h"

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

    LevelMeterData levelMeterData;

private:

    Clipper clipper;
    BitCrusher bitcrusher;
    WaveShaper waveshaper;
    Saturation saturation;

    std::array<juce::dsp::Oversampling<float>, 4> overSamplers
    { {
        {2, 0, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR},
        {2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR},
        {2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR},
        {2, 3, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR},
    } };

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

    //Clipper Params
    juce::AudioParameterInt* clipperSelect{ nullptr };
    juce::AudioParameterFloat* clipperThresh{ nullptr };
    juce::AudioParameterFloat* clipperInGain{ nullptr };
    juce::AudioParameterFloat* clipperOutGain{ nullptr };
    juce::AudioParameterFloat* clipperMix{ nullptr };

    //WaveShaper Params
    juce::AudioParameterInt* waveShaperSelect{ nullptr };
    juce::AudioParameterFloat* waveShaperSin{ nullptr };
    juce::AudioParameterFloat* waveShaperQuadratic{ nullptr };
    juce::AudioParameterFloat* waveShaperFactor{ nullptr };
    juce::AudioParameterFloat* waveShaperGB{ nullptr };
    juce::AudioParameterFloat* waveShaperInGain{ nullptr };
    juce::AudioParameterFloat* waveShaperOutGain{ nullptr };
    juce::AudioParameterFloat* waveShaperMix{ nullptr };
    std::vector<juce::AudioParameterFloat*> waveShaperFactorsHolder{ waveShaperSin, waveShaperQuadratic, waveShaperFactor, waveShaperGB };

    //BitCrusher Params
    juce::AudioParameterInt* crusherBitDepth{ nullptr };
    juce::AudioParameterInt* crusherBitRate{ nullptr };
    juce::AudioParameterFloat* crusherInGain{ nullptr };
    juce::AudioParameterFloat* crusherOutGain{ nullptr };
    juce::AudioParameterFloat* crusherMix{ nullptr };

    //Saturation Params
    juce::AudioParameterFloat* satDrive{ nullptr };
    juce::AudioParameterFloat* satInGain{ nullptr };
    juce::AudioParameterFloat* satOutGain{ nullptr };
    juce::AudioParameterFloat* satMix{ nullptr };

    //Todo
    //Get waveShaper working
        //Add in mix functionality

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistorKAudioProcessor)
};
