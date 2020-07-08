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

class FemtoSynthVoice : public juce::SynthesiserVoice
{
public:
	std::unique_ptr<CorePSG> generator;

	FemtoSynthVoice()
	{
		generator = std::make_unique<CorePSG>();
		generator->setSampleRate(this->getSampleRate());
	}
	~FemtoSynthVoice() {}

	bool canPlaySound(juce::SynthesiserSound* sound) override;
	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
	void stopNote(float velocity, bool allowTailOff) override;
	void pitchWheelMoved(int newPitchWheelValue) override;
	void controllerMoved(int controllerNumber, int newControllerValue) override;
	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
};
