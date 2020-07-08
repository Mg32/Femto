/*
  ==============================================================================

	CorePSG.cpp
	Created: 7 Jul 2020 11:29:08pm
	Author:  moge3

  ==============================================================================
*/

#include "CorePSG.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>

namespace
{
	int32_t frequencyToPeriod(double freq, double simulationFs)
	{
		if (freq < 1)
			return 0xfff;

		int32_t p = round(simulationFs / 2 / freq);

		if (p < 1)
			p = 1;
		if (p > 0xfff)
			p = 0xfff;
		return p;
	}

	double volumeToAmplitude(int32_t internalVolume)
	{
		if (internalVolume < 1)
			return 0;
		if (internalVolume >= 31)
			return 1;
		return pow(2, ((double)internalVolume - 31) / 4);
	}
};

double CorePSG::renderNextSample()
{
	const double timeShift = simulationFs / outputFs;
	time += timeShift;

	const int32_t pulsePeriodA = frequencyToPeriod(pulseFrequencyA * pitchBendFactor, simulationFs);
	const int32_t pulsePeriodB = frequencyToPeriod(pulseFrequencyB * pitchBendFactor, simulationFs);
	const int32_t pulsePeriodC = frequencyToPeriod(pulseFrequencyC * pitchBendFactor, simulationFs);

	// generate samples at the clock rate
	const int32_t samplesToGenerate = ceil(time) - 1;
	for (int32_t j = 0; j < samplesToGenerate; j++)
	{
		const double signalA = pulseA.renderNextSample(pulsePeriodA);
		const double signalB = pulseB.renderNextSample(pulsePeriodB);
		const double signalC = pulseC.renderNextSample(pulsePeriodC);
		const double signalNoise = noise.renderNextSample(noisePeriod);

		const int32_t volumeEnvelope = envelope.renderNextSample(envelopePeriod, isContinued, isAttenuated, isAlternated, isHolded);
		const int32_t volumeChannelA = (volumeA == 0) ? 0 : volumeA * 2 + 1;
		const int32_t volumeChannelB = (volumeB == 0) ? 0 : volumeB * 2 + 1;
		const int32_t volumeChannelC = (volumeC == 0) ? 0 : volumeC * 2 + 1;
		float signal = 0;
		if (noteOnA)
		{
			float amp = volumeToAmplitude(enableEnvelopeA ? volumeEnvelope : volumeChannelA);
			if (enablePulseA)
				signal += amp * signalA;
			if (enableNoiseA)
				signal += amp * signalNoise;
		}
		if (noteOnB)
		{
			float amp = volumeToAmplitude(enableEnvelopeB ? volumeEnvelope : volumeChannelB);
			if (enablePulseB)
				signal += amp * signalB;
			if (enableNoiseB)
				signal += amp * signalNoise;
		}
		if (noteOnC)
		{
			float amp = volumeToAmplitude(enableEnvelopeC ? volumeEnvelope : volumeChannelC);
			if (enablePulseC)
				signal += amp * signalC;
			if (enableNoiseC)
				signal += amp * signalNoise;
		}
		signal /= 6;

		head = (head + 1) % kCircularBufferSize;
		circularBuffer[head] = signal;
	}
	time -= samplesToGenerate;

	// rough interpolation
	double interpolated = 0;
	double denominator = 0;
	for (int32_t j = 0; j < floor(2 * timeShift); j++)
	{
		// triangular weighting
		const double x = ((double)j - time) / timeShift + 1;
		const double weight = (x > 1 || x < -1) ? 0 : (1 - fabs(x));

		// weighted mean
		interpolated += weight * circularBuffer[(kCircularBufferSize + head - j) % kCircularBufferSize];
		denominator += weight;
	}
	double output = interpolated / denominator;

	return output;
}

void CorePSG::noteOn(Channel channel)
{
	envelope.reset();
	switch (channel)
	{
	default:
		break;
	case Channel::ChannelA:
		noteOnA = true;
		break;
	case Channel::ChannelB:
		noteOnB = true;
		break;
	case Channel::ChannelC:
		noteOnC = true;
		break;
	}
}

void CorePSG::noteOff(Channel channel)
{
	switch (channel)
	{
	default:
		break;
	case Channel::ChannelA:
		noteOnA = false;
		break;
	case Channel::ChannelB:
		noteOnB = false;
		break;
	case Channel::ChannelC:
		noteOnC = false;
		break;
	}
}

void CorePSG::setPulseFrequency(Channel channel, double frequency)
{
	switch (channel)
	{
	default:
		break;
	case Channel::ChannelA:
		pulseFrequencyA = frequency;
		break;
	case Channel::ChannelB:
		pulseFrequencyB = frequency;
		break;
	case Channel::ChannelC:
		pulseFrequencyC = frequency;
		break;
	}
}

void CorePSG::setPulseChannelEnabled(Channel channel, bool isEnabled)
{
	switch (channel)
	{
	default:
		break;
	case Channel::ChannelA:
		enablePulseA = isEnabled;
		break;
	case Channel::ChannelB:
		enablePulseB = isEnabled;
		break;
	case Channel::ChannelC:
		enablePulseC = isEnabled;
		break;
	}
}

void CorePSG::setNoiseChannelEnabled(Channel channel, bool isEnabled)
{
	switch (channel)
	{
	default:
		break;
	case Channel::ChannelA:
		enableNoiseA = isEnabled;
		break;
	case Channel::ChannelB:
		enableNoiseB = isEnabled;
		break;
	case Channel::ChannelC:
		enableNoiseC = isEnabled;
		break;
	}
}

void CorePSG::setEnableEnvelope(Channel channel, bool isEnabled)
{
	envelope.reset();
	switch (channel)
	{
	default:
		break;
	case Channel::ChannelA:
		enableEnvelopeA = isEnabled;
		break;
	case Channel::ChannelB:
		enableEnvelopeB = isEnabled;
		break;
	case Channel::ChannelC:
		enableEnvelopeC = isEnabled;
		break;
	}
}

void CorePSG::setChannelVolume(Channel channel, int32_t volume)
{
	switch (channel)
	{
	default:
		break;
	case Channel::ChannelA:
		volumeA = volume;
		break;
	case Channel::ChannelB:
		volumeB = volume;
		break;
	case Channel::ChannelC:
		volumeC = volume;
		break;
	}
}
