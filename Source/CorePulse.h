/*
  ==============================================================================

	CorePulse.h
	Created: 7 Jul 2020 11:29:50pm
	Author:  moge3

  ==============================================================================
*/

#pragma once

#include <cstdint>

class CorePulse
{
public:
	double renderNextSample(int32_t period);

private:
	int32_t counter = 0;
	double previousSample = 0;
};
