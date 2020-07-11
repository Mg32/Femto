/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "FemtoParameterDefinition.h"

//==============================================================================
FemtoAudioProcessor::FemtoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
	, parameters(*this, nullptr, juce::Identifier("APVTS_Femto"), {
			std::make_unique<juce::AudioParameterFloat>(paramdefs::kGainId, "Gain", 0.0f, 1.0f, 0.8f),
			std::make_unique<juce::AudioParameterChoice>(paramdefs::kClockFrequencyId, "Clock freq.", juce::StringArray({ "1.7897725 MHz", "2 MHz" }), 0),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnablePulseChAId, "Enable Pulse Ch.A", true),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnablePulseChBId, "Enable Pulse Ch.B", false),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnablePulseChCId, "Enable Pulse Ch.C", false),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnableNoiseChAId, "Enable Noise Ch.A", false),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnableNoiseChBId, "Enable Noise Ch.B", false),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnableNoiseChCId, "Enable Noise Ch.C", false),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnableEnvelopeChAId, "Enable Env Ch.A", false),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnableEnvelopeChBId, "Enable Env Ch.B", false),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnableEnvelopeChCId, "Enable Env Ch.C", false),
			std::make_unique<juce::AudioParameterInt>(paramdefs::kVolumeChAId, "Volume Ch.A", 0, 15, 15),
			std::make_unique<juce::AudioParameterInt>(paramdefs::kVolumeChBId, "Volume Ch.B", 0, 15, 15),
			std::make_unique<juce::AudioParameterInt>(paramdefs::kVolumeChCId, "Volume Ch.C", 0, 15, 15),
			std::make_unique<juce::AudioParameterInt>(paramdefs::kNoisePeriodId, "Noise Period", 1, 31, 15),
			std::make_unique<juce::AudioParameterInt>(paramdefs::kEnvelopePeriodId, "Env Period", 1, 0xffff, 0x800),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnvelopeCNTId, "Env CNT", false),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnvelopeATTId, "Env ATT", false),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnvelopeALTId, "Env ALT", false),
			std::make_unique<juce::AudioParameterBool>(paramdefs::kEnvelopeHLDId, "Env HLD", false),
		})
{
	synth = std::make_unique<FemtoSynthesizer>(parameters);

	gainParameter = parameters.getRawParameterValue(paramdefs::kGainId);
}

FemtoAudioProcessor::~FemtoAudioProcessor()
{
}

//==============================================================================
const juce::String FemtoAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool FemtoAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool FemtoAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool FemtoAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double FemtoAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int FemtoAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int FemtoAudioProcessor::getCurrentProgram()
{
	return 0;
}

void FemtoAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String FemtoAudioProcessor::getProgramName(int index)
{
	return {};
}

void FemtoAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void FemtoAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	synth->setCurrentPlaybackSampleRate(sampleRate);
}

void FemtoAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FemtoAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
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

void FemtoAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	synth->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
	buffer.applyGain(*gainParameter);
}

//==============================================================================
bool FemtoAudioProcessor::hasEditor() const
{
	return false; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FemtoAudioProcessor::createEditor()
{
	return new FemtoAudioProcessorEditor(*this);
}

//==============================================================================
void FemtoAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	auto root = parameters.state.createXml();
	copyXmlToBinary(*root, destData);
}

void FemtoAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	auto root = getXmlFromBinary(data, sizeInBytes);
	if (!root || !root->hasTagName(parameters.state.getType()))
	{
		return;
	}

	parameters.state = juce::ValueTree::fromXml(*root);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new FemtoAudioProcessor();
}
