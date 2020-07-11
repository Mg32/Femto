/*
  ==============================================================================

	FemtoParameterDefinition.h
	Created: 11 Jul 2020 10:44:03pm
	Author:  moge3

  ==============================================================================
*/

#pragma once
#include <cstdint>

namespace paramdefs
{
	// ID
	static const char* kGainId = "Gain";
	static const char* kClockFrequencyId = "ClockFreq";
	static const char* kEnablePulseChAId = "PulseChA";
	static const char* kEnablePulseChBId = "PulseChB";
	static const char* kEnablePulseChCId = "PulseChC";
	static const char* kEnableNoiseChAId = "NoiseChA";
	static const char* kEnableNoiseChBId = "NoiseChB";
	static const char* kEnableNoiseChCId = "NoiseChC";
	static const char* kEnableEnvelopeChAId = "EnvChA";
	static const char* kEnableEnvelopeChBId = "EnvChB";
	static const char* kEnableEnvelopeChCId = "EnvChC";
	static const char* kVolumeChAId = "VolChA";
	static const char* kVolumeChBId = "VolChB";
	static const char* kVolumeChCId = "VolChC";
	static const char* kNoisePeriodId = "NoisePeriod";
	static const char* kEnvelopeCNTId = "EnvCNT";
	static const char* kEnvelopeATTId = "EnvATT";
	static const char* kEnvelopeALTId = "EnvALT";
	static const char* kEnvelopeHLDId = "EnvHLD";

	// Clock Frequency
	static const size_t kNumClockFrequencies = 2;
	static const float kClockFrequencies[kNumClockFrequencies] = {
		1.7897725e6f,
		2e6f,
	};
}
