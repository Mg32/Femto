/*
  ==============================================================================

	CoreEnvelope.h
	Created: 8 Jul 2020 1:22:37am
	Author:  moge3

  ==============================================================================
*/

#pragma once

#include <cstdint>

class CoreEnvelope
{
public:
	void reset()
	{
		phaseCounter = 0;
		loopCounter = 0;
	}
	int32_t renderNextSample(int32_t period, bool isContinued, bool isAttenuated, bool isAlternated, bool isHold);

private:
	int32_t phaseCounter = 0;
	uint32_t loopCounter = 0;
};
