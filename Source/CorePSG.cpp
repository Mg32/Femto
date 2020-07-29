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
#include <algorithm>

namespace
{
	int32_t frequencyToPeriod(double freq, double simulationFs)
	{
		if (freq < 1)
			return 0xfff;

		int32_t period = round(simulationFs / 2 / freq);

		if (period < 1)
			period = 1;
		if (period > 0xfff)
			period = 0xfff;
		return period;
	}

	double volumeToAmplitude(int32_t internalVolume)
	{
		if (internalVolume < 1)
			return 0;
		if (internalVolume >= 31)
			return 1;
		return pow(2, ((double)internalVolume - 31) / 4);
	}

	double sinc(double x)
	{
		constexpr double eps = std::numeric_limits<double>::epsilon();
		const double theta = M_PI * x + eps;
		return sin(theta) / theta;
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

	// sinc interpolation
	double interpolated = 0;
	for (int j = 0; j < kCircularBufferSize; j++)
	{
		const double pos = static_cast<double>(j) - kHalfBufferSize + time;
		
		const double tableIndexReal = pos / kHalfBufferSize * kWeightTableMax;
		int32_t tableIndex = floor(tableIndexReal);
		double tableDelta = tableIndexReal - tableIndex;
		if (tableIndex < 0)
		{
			tableIndex = -tableIndex;
			tableDelta = 1 - tableDelta;
		}
		if (tableIndex + 1 > kWeightTableMax)
		{
			continue;
		}

		const int32_t bufferPosition = (kCircularBufferSize + head - j) % kCircularBufferSize;
		const double weight = (1 - tableDelta) * weightTable[tableIndex] + tableDelta * weightTable[tableIndex + 1];
		interpolated += weight * circularBuffer[bufferPosition];
	}

	return interpolated;
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

void CorePSG::setPulseEnabled(Channel channel, bool isEnabled)
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

void CorePSG::setNoiseEnabled(Channel channel, bool isEnabled)
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

void CorePSG::setEnvelopeEnabled(Channel channel, bool isEnabled)
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

void CorePSG::updateWeightTable()
{
	const double factor = std::min(outputFs / simulationFs, 1.0);

	for (int32_t i = 0; i < kWeightTableSize; i++)
	{
		const double x = static_cast<double>(i) / kWeightTableMax;
		const double t = factor * kHalfBufferSize * x;

		const double theta = M_PI * x;
		const double window = 0.42 + 0.5 * cos(theta) + 0.08 * cos(2 * theta);

		weightTable[i] = factor * sinc(t) * window;
		if (theta > M_PI || theta < -M_PI)
		{
			// out of window
			weightTable[i] = 0;
		}
	}
}
