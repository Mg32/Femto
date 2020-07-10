/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
{
	synth = new FemtoSynthesizer(*this);

	// parameters
	addParameter(gain = new juce::AudioParameterFloat(
		"gain",
		"Gain",
		juce::NormalisableRange<float>(0.0f, 1.0f),
		1.0f
	));
}

FemtoAudioProcessor::~FemtoAudioProcessor()
{
	delete synth;
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
	buffer.applyGain(*gain);
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
	juce::XmlElement root("State");


	for (int i = 0; i < getNumParameters(); i++)
	{
		juce::XmlElement* el = root.createNewChildElement(getParameterName(i));
		el->addTextElement(juce::String(getParameter(i)));
	}

	copyXmlToBinary(root, destData);
}

void FemtoAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	auto root = getXmlFromBinary(data, sizeInBytes);
	if (!root)
	{
		return;
	}

	forEachXmlChildElement((*root), child)
	{
		for (int i = 0; i < getNumParameters(); i++)
		{
			if (!child->hasTagName(getParameterName(i)))
				continue;

			juce::String text = child->getAllSubText();
			setParameter(i, text.getFloatValue());
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new FemtoAudioProcessor();
}
