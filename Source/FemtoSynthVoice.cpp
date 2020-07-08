/*
  ==============================================================================

	FemtoSynthVoice.cpp
	Created: 7 Jul 2020 10:00:45pm
	Author:  moge3

  ==============================================================================
*/

#include "FemtoSynthVoice.h"
#include "FemtoSynthSound.h"

bool FemtoSynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
	return dynamic_cast<FemtoSynthSound*>(sound) != nullptr;
}

void FemtoSynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
	double frequency = 440.0 * pow(2.0, ((double)midiNoteNumber - 69) / 12.0);
	generator->setPulseFrequency(CorePSG::Channel::ChannelA, frequency);
	generator->noteOn(CorePSG::Channel::ChannelA);
}

void FemtoSynthVoice::stopNote(float velocity, bool allowTailOff)
{
	generator->noteOff(CorePSG::Channel::ChannelA);
}

void FemtoSynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
	double normalized = ((double)newPitchWheelValue - 8192.0) / 8192.0;
	double bend = pow(2.0, normalized / 12.0);
	generator->setPitchBend(bend);
}

void FemtoSynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

void FemtoSynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
	for (int sample = startSample; sample < numSamples + startSample; sample++)
	{
		float signal = generator->renderNextSample();

		for (int ch = 0; ch < outputBuffer.getNumChannels(); ch++)
		{
			outputBuffer.addSample(ch, sample, signal);
		}
	}
}
