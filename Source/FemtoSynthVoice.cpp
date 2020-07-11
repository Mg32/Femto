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

void FemtoSynthVoice::Listener::parameterChanged(const juce::String& parameterID, float newValue)
{
	if (parameterID == paramdefs::kClockFrequencyId)
	{
		double freq = paramdefs::kClockFrequencies[static_cast<int>(newValue)];
		generator.setClockFrequency(freq);
	}

	if (parameterID == paramdefs::kEnablePulseChAId)
		generator.setPulseEnabled(CorePSG::Channel::ChannelA, static_cast<bool>(newValue));
	if (parameterID == paramdefs::kEnablePulseChBId)
		generator.setPulseEnabled(CorePSG::Channel::ChannelB, static_cast<bool>(newValue));
	if (parameterID == paramdefs::kEnablePulseChCId)
		generator.setPulseEnabled(CorePSG::Channel::ChannelC, static_cast<bool>(newValue));

	if (parameterID == paramdefs::kEnableNoiseChAId)
		generator.setNoiseEnabled(CorePSG::Channel::ChannelA, static_cast<bool>(newValue));
	if (parameterID == paramdefs::kEnablePulseChBId)
		generator.setNoiseEnabled(CorePSG::Channel::ChannelB, static_cast<bool>(newValue));
	if (parameterID == paramdefs::kEnablePulseChCId)
		generator.setNoiseEnabled(CorePSG::Channel::ChannelC, static_cast<bool>(newValue));

	if (parameterID == paramdefs::kEnableEnvelopeChAId)
		generator.setEnvelopeEnabled(CorePSG::Channel::ChannelA, static_cast<bool>(newValue));
	if (parameterID == paramdefs::kEnableEnvelopeChBId)
		generator.setEnvelopeEnabled(CorePSG::Channel::ChannelB, static_cast<bool>(newValue));
	if (parameterID == paramdefs::kEnableEnvelopeChCId)
		generator.setEnvelopeEnabled(CorePSG::Channel::ChannelC, static_cast<bool>(newValue));

	if (parameterID == paramdefs::kVolumeChAId)
		generator.setChannelVolume(CorePSG::Channel::ChannelA, static_cast<int>(newValue));
	if (parameterID == paramdefs::kVolumeChBId)
		generator.setChannelVolume(CorePSG::Channel::ChannelB, static_cast<int>(newValue));
	if (parameterID == paramdefs::kVolumeChCId)
		generator.setChannelVolume(CorePSG::Channel::ChannelC, static_cast<int>(newValue));

	if (parameterID == paramdefs::kNoisePeriodId)
		generator.setNoisePeriod(static_cast<int>(newValue));
	if (parameterID == paramdefs::kEnvelopePeriodId)
		generator.setEnvelopePeriod(static_cast<int>(newValue));

	if (parameterID == paramdefs::kEnvelopeCNTId)
		generator.setEnvelopeIsContinued(static_cast<bool>(newValue));
	if (parameterID == paramdefs::kEnvelopeATTId)
		generator.setEnvelopeIsAttenuated(static_cast<bool>(newValue));
	if (parameterID == paramdefs::kEnvelopeALTId)
		generator.setEnvelopeIsAlternated(static_cast<bool>(newValue));
	if (parameterID == paramdefs::kEnvelopeHLDId)
		generator.setEnvelopeIsHolded(static_cast<bool>(newValue));
}
