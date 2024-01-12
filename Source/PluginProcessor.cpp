/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MohaAudioProcessor::MohaAudioProcessor()
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
}

MohaAudioProcessor::~MohaAudioProcessor()
{
}

//==============================================================================
const juce::String MohaAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MohaAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MohaAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MohaAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MohaAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MohaAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MohaAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MohaAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MohaAudioProcessor::getProgramName (int index)
{
    return {};
}

void MohaAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MohaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    moha_fx.prepare(spec);
}

void MohaAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MohaAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void MohaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //define parameters in relation to the audio processor value tree state
    float gain = *apvts.getRawParameterValue("Gain");
    float pre_highPassFreq = *apvts.getRawParameterValue("HighPassFreq");
    float pre_lowPassFreq = *apvts.getRawParameterValue("LowPassFreq");
    float toneFreq = *apvts.getRawParameterValue("ToneFreq");
    float sensitivity = *apvts.getRawParameterValue("Sensitivity");
    float speed = *apvts.getRawParameterValue("Speed");
    float darkness = *apvts.getRawParameterValue("Darkness");
    float volume = *apvts.getRawParameterValue("Volume");

    // Renew parameters
    moha_fx.SetGain(gain);
    moha_fx.SetHPFPreFreq(pre_highPassFreq);
    moha_fx.SetLPFPreFreq(pre_lowPassFreq);
    moha_fx.SetSensitivity(sensitivity);
    moha_fx.SetSpeed(speed);
    moha_fx.SetDarkness(darkness);
    moha_fx.SetVolume(volume);
    moha_fx.SetTone(toneFreq);

    juce::dsp::AudioBlock<float> block(buffer);
    moha_fx.process(block);

    float loopSamples = *apvts.getRawParameterValue("LoopSamples");
    bool loopSwitch = *apvts.getRawParameterValue("LoopSwitch");

	circularBuffer.fillBuffer(buffer);

    if (loopSwitch == true)
    {
        circularBuffer.resetBufferFromCircularBuffer(buffer, loopSamples);
	}
    else
    {
        circularBuffer.isLooping = false;
    }
}

//==============================================================================
bool MohaAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MohaAudioProcessor::createEditor()
{
    return new MohaAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void MohaAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MohaAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

using APVTS = juce::AudioProcessorValueTreeState;
APVTS::ParameterLayout MohaAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;

    using namespace juce;

    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ "Gain", 1 },
        "Gain",
        NormalisableRange<float>(-24.f, 24.f, .1f, 1.f), 0.f));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ "HighPassFreq", 1 },
        "HighPassFreq",
        NormalisableRange<float>(20, 5000, .1, 0.5), 20));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ "LowPassFreq", 1 },
        "LowPassFreq",
        NormalisableRange<float>(100, 20000, .1, 0.5), 20000));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ "ToneFreq", 1 },
        "ToneFreq",
        NormalisableRange<float>(500, 20000, .1, 0.25), 16000));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ "Sensitivity", 1 },
        "Sensitivity",
        NormalisableRange<float>(0.f, 100.f, 0.1f, 0.5), 25.f));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ "Speed", 1 },
        "Speed",
        NormalisableRange<float>(0.f, 100.f, 0.1f, 1.f), 50.f));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ "Darkness", 1 },
        "Darkness",
        NormalisableRange<float>(0.f, 100.f, 0.1f, 1.f), 50.f));
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ "Volume", 1 },
        "Volume",
        NormalisableRange<float>(-MAX_GAIN_RANGE, MAX_GAIN_RANGE, .1f, 1.f), 0.f));

    layout.add(std::make_unique<AudioParameterFloat>(ParameterID{ "LoopSamples", 1 },
        "LoopSamples",
        NormalisableRange<float>(512, 96000, 1, 1.f), 24000));

    layout.add(std::make_unique<AudioParameterBool>(ParameterID{ "LoopSwitch", 1 },
        "LoopSwitch",
        false));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MohaAudioProcessor();
}
