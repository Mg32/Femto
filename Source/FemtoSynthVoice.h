/*
  ==============================================================================

	FemtoSynthVoice.h
	Created: 7 Jul 2020 10:00:45pm
	Author:  moge3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CorePSG.h"
#include "FemtoParameterDefinition.h"

class FemtoSynthVoice : public juce::SynthesiserVoice
{
public:
	FemtoSynthVoice(juce::AudioProcessorValueTreeState& parameters)
	{
		generator = std::make_unique<CorePSG>();
		generator->setSampleRate(this->getSampleRate());

		listener = new Listener(*generator);
		parameters.addParameterListener(paramdefs::kClockFrequencyId, listener);
		parameters.addParameterListener(paramdefs::kEnablePulseChAId, listener);
		parameters.addParameterListener(paramdefs::kEnablePulseChBId, listener);
		parameters.addParameterListener(paramdefs::kEnablePulseChCId, listener);
		parameters.addParameterListener(paramdefs::kEnableNoiseChAId, listener);
		parameters.addParameterListener(paramdefs::kEnableNoiseChBId, listener);
		parameters.addParameterListener(paramdefs::kEnableNoiseChCId, listener);
		parameters.addParameterListener(paramdefs::kEnableEnvelopeChAId, listener);
		parameters.addParameterListener(paramdefs::kEnableEnvelopeChBId, listener);
		parameters.addParameterListener(paramdefs::kEnableEnvelopeChCId, listener);
		parameters.addParameterListener(paramdefs::kVolumeChAId, listener);
		parameters.addParameterListener(paramdefs::kVolumeChBId, listener);
		parameters.addParameterListener(paramdefs::kVolumeChCId, listener);
		parameters.addParameterListener(paramdefs::kNoisePeriodId, listener);
		parameters.addParameterListener(paramdefs::kEnvelopePeriodId, listener);
		parameters.addParameterListener(paramdefs::kEnvelopeCNTId, listener);
		parameters.addParameterListener(paramdefs::kEnvelopeATTId, listener);
		parameters.addParameterListener(paramdefs::kEnvelopeALTId, listener);
		parameters.addParameterListener(paramdefs::kEnvelopeHLDId, listener);
	}
	~FemtoSynthVoice()
	{
		delete listener;
	}

	bool canPlaySound(juce::SynthesiserSound* sound) override;
	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
	void stopNote(float velocity, bool allowTailOff) override;
	void pitchWheelMoved(int newPitchWheelValue) override;
	void controllerMoved(int controllerNumber, int newControllerValue) override;
	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
	struct Listener : juce::AudioProcessorValueTreeState::Listener
	{
	public:
		Listener(CorePSG& g) : generator(g) { }
		void parameterChanged(const juce::String& parameterID, float newValue) override;

	private:
		CorePSG& generator;
	};

	std::unique_ptr<CorePSG> generator;
	Listener* listener;
};
