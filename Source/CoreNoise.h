/*
  ==============================================================================

	CoreNoise.h
	Created: 7 Jul 2020 11:29:33pm
	Author:  moge3

  ==============================================================================
*/

#pragma once

#include <cstdint>

class CoreNoise
{
public:
    CoreNoise();
    double renderNextSample(int32_t period);

private:
    int32_t counter = 0;
    uint32_t bits = 0;
    double previousSample = 0;
};
