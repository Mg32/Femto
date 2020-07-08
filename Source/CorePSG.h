/*
  ==============================================================================

	CorePSG.h
	Created: 7 Jul 2020 11:29:08pm
	Author:  moge3

  ==============================================================================
*/

#pragma once
#include "CorePulse.h"
#include "CoreNoise.h"
#include "CoreEnvelope.h"

class CorePSG
{
public:
	double renderNextSample();

	void setSampleRate(double frequency) { outputFs = frequency; }
	void setClockFrequency(double frequency) { simulationFs = 2 * frequency / 16; }

	enum class Channel { ChannelA, ChannelB, ChannelC };

	void noteOn(Channel channel);
	void noteOff(Channel channel);

	void setPulseFrequency(Channel channel, double frequency);
	void setPulseChannelEnabled(Channel channel, bool isEnabled);
	void setNoiseChannelEnabled(Channel channel, bool isEnabled);
	void setEnableEnvelope(Channel channel, bool isEnabled);
	void setChannelVolume(Channel channel, int32_t volume);

	void setPitchBend(double factor) { pitchBendFactor = factor; }
	void setNoisePeriod(int32_t period) { noisePeriod = period; }

	void setEnvelopePeriod(int32_t period) { envelopePeriod = period; }
	void setEnvelopeIsContinued(bool isContinued) { this->isContinued = isContinued; }
	void setEnvelopeIsAttenuated(bool isAttenuated) { this->isAttenuated = isAttenuated; }
	void setEnvelopeIsAlternated(bool isAlternated) { this->isAlternated = isAlternated; }
	void setEnvelopeIsHolded(bool isHolded) { this->isHolded = isHolded; }

private:
	double outputFs = 48000;
	double simulationFs = 111860.78125; // 2 * 1.7897725e6 / 16

	// voicing
	bool noteOnA = false;
	bool noteOnB = false;
	bool noteOnC = false;

	// channel mixing
	bool enablePulseA = true;
	bool enablePulseB = false;
	bool enablePulseC = false;
	bool enableNoiseA = false;
	bool enableNoiseB = false;
	bool enableNoiseC = false;
	bool enableEnvelopeA = false;
	bool enableEnvelopeB = false;
	bool enableEnvelopeC = false;
	int32_t volumeA = 15;
	int32_t volumeB = 15;
	int32_t volumeC = 15;

	// pulse
	CorePulse pulseA;
	CorePulse pulseB;
	CorePulse pulseC;
	double pulseFrequencyA = 440;
	double pulseFrequencyB = 220;
	double pulseFrequencyC = 110;
	double pitchBendFactor = 1;

	// noise
	CoreNoise noise;
	int32_t noisePeriod = 31;

	// envelope
	CoreEnvelope envelope;
	int32_t envelopePeriod = 0x800;
	bool isContinued = false;
	bool isAttenuated = false;
	bool isAlternated = false;
	bool isHolded = false;

	// interpolation
	static const int32_t kCircularBufferSize = 64;
	double circularBuffer[kCircularBufferSize] = { };
	int32_t head = 0;
	double time = 0;
};
