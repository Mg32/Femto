/*
  ==============================================================================

	CoreNoise.cpp
	Created: 7 Jul 2020 11:29:33pm
	Author:  moge3

  ==============================================================================
*/

#include "CoreNoise.h"

CoreNoise::CoreNoise()
{
    // warm up the internal bits
    for (int i = 0; i < 17 * 2; i++)
    {
        renderNextSample(1);
    }
}

double CoreNoise::renderNextSample(int32_t period)
{
    int32_t actualPeriod = period;
    if (actualPeriod < 1)
        actualPeriod = 1;
    if (actualPeriod > 31)
        actualPeriod = 31;
    actualPeriod *= 2;

    // differenciated PSG noise
    float signal = 0;
    if (counter % actualPeriod == 0)
    {
        // pseudo random generation using xor
        const uint32_t bitNext = (bits >> 16 & 0x01) ^ (bits >> 13 & 0x01) ^ (bits & 0x01) ^ 1;
        bits = (bitNext << 16) | (bits >> 1);

        // random bipolar impulses
        const uint32_t prev = (bits >> 1 & 0x01);
        const uint32_t curr = (bits & 0x01);
        signal = (curr > prev) ? 1 : (curr < prev) ? -1 : 0;
    }
    counter = (counter + 1) % actualPeriod;

    // integration
    previousSample = signal + 0.999 * previousSample;
    return previousSample;
}
