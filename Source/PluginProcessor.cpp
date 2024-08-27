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
    globalBypass = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("globalBypass"));
    selectClip = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectClip"));
    selectBit = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectBit"));
    selectWaveShpr = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectWaveShpr"));
    selectSat = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("selectSat"));
    masterInValue = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("masterInValue"));
    masterOutValue = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("masterOutValue"));
    masterMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("masterMix"));
    overSampleSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("overSampleSelect"));
    distorkEngine.engineToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("toggleEngine"));

    //Clipper Controls
    distorkEngine.clipperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("toggleClip"));
    distorkEngine.clipperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("clipperSelect"));
    distorkEngine.clipperThresh = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("clipperThresh"));
    distorkEngine.clipperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("clipperInGain"));
    distorkEngine.clipperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("clipperOutGain"));
    distorkEngine.clipperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("clipperMix"));

    //WaveShaper Controls
    distorkEngine.waveShaperToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("toggleWaveShpr"));
    distorkEngine.waveShaperSelect = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("waveShaperSelect"));
    distorkEngine.waveShaperFactorsHolder[0] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("waveShaperSin"));
    distorkEngine.waveShaperFactorsHolder[1] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("waveShaperQuadratic"));
    distorkEngine.waveShaperFactorsHolder[2] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("waveShaperFactor"));
    distorkEngine.waveShaperFactorsHolder[3] = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("waveShaperGB"));
    distorkEngine.waveShaperInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("waveShaperInGain"));
    distorkEngine.waveShaperOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("waveShaperOutGain"));
    distorkEngine.waveShaperMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("waveShaperMix"));

    //BitCrusher Controls
    distorkEngine.crusherToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("toggleBit"));
    distorkEngine.crusherBitDepth = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("crusherBitDepth"));
    distorkEngine.crusherBitRate = dynamic_cast<juce::AudioParameterInt*>(apvts.getParameter("crusherBitRate"));
    distorkEngine.crusherInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("crusherInGain"));
    distorkEngine.crusherOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("crusherOutGain"));
    distorkEngine.crusherMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("crusherMix"));

    //Saturator Controls
    distorkEngine.satToggle = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("toggleSat"));
    distorkEngine.satDrive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("satDrive"));
    distorkEngine.satInGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("satInGain"));
    distorkEngine.satOutGain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("satOutGain"));
    distorkEngine.satMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("satMix"));
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

    //distorkEngine.prepareToPlay(spec);
    bitcrusher.prepareToPlay(spec);
    clipper.prepareToPlay(spec);
    saturation.prepareToPlay(spec);
    waveshaper.prepareToPlay(spec);

    satIn.reset();
    satIn.prepare(spec);
    satIn.setRampDurationSeconds(.05);
    satOut.reset();
    satOut.prepare(spec);
    satOut.setRampDurationSeconds(.05);
    clipperIn.reset();
    clipperIn.prepare(spec);
    clipperIn.setRampDurationSeconds(.05);
    clipperOut.reset();
    clipperOut.prepare(spec);
    clipperOut.setRampDurationSeconds(.05);
    bitCrusherIn.reset();
    bitCrusherIn.prepare(spec);
    bitCrusherIn.setRampDurationSeconds(.05);
    bitCrusherOut.reset();
    bitCrusherOut.prepare(spec);
    bitCrusherOut.setRampDurationSeconds(.05);
    waveShaperIn.reset();
    waveShaperIn.prepare(spec);
    waveShaperIn.setRampDurationSeconds(.05);
    waveShaperOut.reset();
    waveShaperOut.prepare(spec);
    waveShaperOut.setRampDurationSeconds(.05);

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

    if (globalBypass->get()) { return; }

    //copy buffer for master dry/wet knob
    juce::AudioBuffer<float> copyBuffer;
    copyBuffer.makeCopyOf(buffer);

    auto inputBlock = juce::dsp::AudioBlock<float>(buffer);
    auto copyBlock = juce::dsp::AudioBlock<float>(copyBuffer);
    auto inputContext = juce::dsp::ProcessContextReplacing<float>(inputBlock);

    masterIn.setGainDecibels(masterInValue->get());
    masterIn.process(inputContext);

    levelMeterData.process(true, 0, buffer);
    levelMeterData.process(true, 1, buffer);

    auto ovRate = overSampleSelect->get();

    distorkEngine.process(inputContext, distortionProcessOrder, ovRate, overSamplers);

    masterOut.setGainDecibels(masterOutValue->get());
    masterOut.process(inputContext);
 
    //process master dry/wet
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        auto data = inputBlock.getChannelPointer(ch);
        auto dry = copyBlock.getChannelPointer(ch);

        for (int s = 0; s < inputBlock.getNumSamples(); s++)
        {
            data[s] = (data[s] * masterMix->get() / 100) + (dry[s] * (1 - masterMix->get() / 100));
        }
    }

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
    auto interGainRange = NormalisableRange<float>(-6, 6, .1, 1);
    auto mixRange = NormalisableRange<float>(0, 100, 1, 1);

    //Master Controls
    layout.add(std::make_unique<AudioParameterBool>("toggleEngine", "Engine Toggle", false));
    layout.add(std::make_unique<AudioParameterBool>("globalBypass", "Global Bypass", false));
    layout.add(std::make_unique<AudioParameterBool>("selectClip", "Clipper", false));
    layout.add(std::make_unique<AudioParameterBool>("selectBit", "BitCrusher", false));
    layout.add(std::make_unique<AudioParameterBool>("selectWaveShpr", "WaveShaper", false));
    layout.add(std::make_unique<AudioParameterBool>("selectSat", "Saturation", true));
    layout.add(std::make_unique<AudioParameterFloat>("masterInValue", "Input", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("masterOutValue", "Output", gainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("masterMix", "Dry/Wet", mixRange, 100));
    layout.add(std::make_unique<AudioParameterInt>("overSampleSelect", "Oversample Rate", 0, 3, 0));

    //Clipper Controls
    auto threshRange = NormalisableRange<float>(-60, 0, .1, 1);
    layout.add(std::make_unique<AudioParameterBool>("toggleClip", "Toggle Clip", false));
    layout.add(std::make_unique<AudioParameterInt>("clipperSelect", "Clipper Type", 0, 5, 0));
    layout.add(std::make_unique<AudioParameterFloat>("clipperThresh", "Threshold", threshRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("clipperInGain", "In Gain", interGainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("clipperOutGain", "Out Gain", interGainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("clipperMix", "Dry/Wet", mixRange, 100));

    //WaveShaper Controls
    auto lessThanOne = NormalisableRange<float>(.01, .99, .01, 1);
    auto sineFactor = NormalisableRange<float>(.05, .95, .01, 1);
    auto moreThanOne = NormalisableRange<float>(1, 10, .01, 1);
    layout.add(std::make_unique<AudioParameterBool>("toggleWaveShpr", "Toggle WS", false));
    layout.add(std::make_unique<AudioParameterInt>("waveShaperSelect", "Type", 0, 3, 0));
    layout.add(std::make_unique<AudioParameterFloat>("waveShaperSin", "Drive", sineFactor, .05));
    layout.add(std::make_unique<AudioParameterFloat>("waveShaperQuadratic", "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>("waveShaperFactor", "Drive", lessThanOne, .05));
    layout.add(std::make_unique<AudioParameterFloat>("waveShaperGB", "Drive", moreThanOne, 1));
    layout.add(std::make_unique<AudioParameterFloat>("waveShaperInGain", "In Gain", interGainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("waveShaperOutGain", "Out Gain", interGainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("waveShaperMix", "Dry/Wet", mixRange, 100));

    //BitCrusher Controls
    layout.add(std::make_unique<AudioParameterBool>("toggleBit", "Toggle Bit", false));
    layout.add(std::make_unique<AudioParameterInt>("crusherBitDepth", "Bit Depth", 1, 16, 16));
    layout.add(std::make_unique<AudioParameterInt>("crusherBitRate", "Bit Rate", 1, 25, 1));
    layout.add(std::make_unique<AudioParameterFloat>("crusherInGain", "In Gain", interGainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("crusherOutGain", "Out Gain", interGainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("crusherMix", "Dry/Wet", mixRange, 100));

    //Saturation Controls
    auto driveRange = NormalisableRange<float>(1, 10, .1, 1);
    layout.add(std::make_unique<AudioParameterBool>("toggleSat", "Toggle Sat", false));
    layout.add(std::make_unique<AudioParameterFloat>("satDrive", "Drive", driveRange, 1));
    layout.add(std::make_unique<AudioParameterFloat>("satInGain", "In Gain", interGainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("satOutGain", "Out Gain", interGainRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("satMix", "Dry/Wet", mixRange, 100));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistorKAudioProcessor();
}
