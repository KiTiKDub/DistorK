/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistorKAudioProcessor::DistorKAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    //Master Controls
    bypass = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("bypass"));
    selectClip = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectClip"));
    selectBit = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectBit"));
    selectWaveShpr = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectWaveShpr"));
    selectSat = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectSat"));
    masterInValue = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("masterInValue"));
    masterOutValue = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("masterOutValue"));
    masterMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("masterMix"));
    overSampleSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("overSampleSelect"));

    //Clipper Controls
    clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("clipperSelect"));
    clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("clipperThresh"));
    clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("clipperInGain"));
    clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("clipperOutGain"));
    clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("clipperMix"));

}

DistorKAudioProcessor::~DistorKAudioProcessor()
{
}

//==============================================================================
const juce::String DistorKAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistorKAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistorKAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistorKAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistorKAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistorKAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistorKAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistorKAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistorKAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistorKAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistorKAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumInputChannels();

    masterIn.reset();
    masterIn.prepare(spec);
    masterIn.setRampDurationSeconds(.05);

    masterOut.reset();
    masterOut.prepare(spec);
    masterOut.setRampDurationSeconds(.05);

    clipper.prepareToPlay(spec);

    for (auto& oversample : overSamplers)
    {
        oversample.reset();
        oversample.initProcessing(samplesPerBlock);
    }

}

void DistorKAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistorKAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DistorKAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (bypass->get()) { return; }

    levelMeterData.process(true, 0, buffer);
    levelMeterData.process(true, 1, buffer);

    auto inputBlock = juce::dsp::AudioBlock<float>(buffer);
    auto inputContext = juce::dsp::ProcessContextReplacing<float>(inputBlock);

    masterIn.setGainDecibels(masterInValue->get());
    masterIn.process(inputContext);

    auto ovRate = overSampleSelect->get();

    auto ovBlock = overSamplers[ovRate].processSamplesUp(inputBlock);
    auto ovContext = juce::dsp::ProcessContextReplacing<float>(ovBlock);

    //==============================================================================
    //NON MASTER PROCESSING HERE
    //******************************************************************************

    clipper.updateParams(clipperSelect->get(), clipperThresh->get(), clipperInGain->get(), clipperOutGain->get(), clipperMix->get());
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
        clipper.process(ovBlock, channel);

    //==============================================================================

    auto& outputBlock = inputContext.getOutputBlock();
    overSamplers[ovRate].processSamplesDown(outputBlock);

    masterOut.setGainDecibels(masterOutValue->get());
    masterOut.process(inputContext);

    levelMeterData.process(false, 0, buffer);
    levelMeterData.process(false, 1, buffer);
    
}

//==============================================================================
bool DistorKAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistorKAudioProcessor::createEditor()
{
    return new DistorKAudioProcessorEditor (*this);
}

//==============================================================================
void DistorKAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void DistorKAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout DistorKAudioProcessor::createParameterLayout()
{
    using namespace juce;

    AudioProcessorValueTreeState::ParameterLayout layout;

    //universal ranges
    auto gainRange = NormalisableRange<float>(-24, 24, .1, 1);
    auto zeroToOne = NormalisableRange<float>(0, 1, .01, 1);

    //Master Controls
    layout.add(std::make_unique<AudioParameterBool>("selectClip", "Clipper", false));
    layout.add(std::make_unique<AudioParameterBool>("selectBit", "BitCrusher", false));
    layout.add(std::make_unique<AudioParameterBool>("selectWaveShpr", "WaveShaper", false));
    layout.add(std::make_unique<AudioParameterBool>("selectSat", "Saturation", true));
    layout.add(std::make_unique<AudioParameterFloat>("masterInValue", "Input", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("masterOutValue", "Output", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("masterMix", "Mix", zeroToOne, 1));
    layout.add(std::make_unique<AudioParameterInt>("overSampleSelect", "Oversample Rate", 0, 3, 0));

    //Clipper Controls
    auto threshRange = NormalisableRange<float>(-60, 0, .1, 1);
    layout.add(std::make_unique<AudioParameterInt>("clipperSelect", "Clipper Type", 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>("clipperThresh", "Threshold", threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("clipperInGain", "In Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("clipperOutGain", "Out Gain", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("clipperMix", "Mix", zeroToOne, 1));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistorKAudioProcessor();
}
