/*
  ==============================================================================

	CoreEnvelope.cpp
	Created: 8 Jul 2020 1:22:37am
	Author:  moge3

  ==============================================================================
*/

#include "CoreEnvelope.h"
#include <cmath>

int32_t CoreEnvelope::renderNextSample(int32_t period, bool isContinued, bool isAttenuated, bool isAlternated, bool isHold)
{
    int32_t actualPeriod = period;
    if (actualPeriod < 1)
        actualPeriod = 1;
    if (actualPeriod > 0xffff)
        actualPeriod = 0xffff;
    actualPeriod *= 32;

    // ramp wave
    double envelope = phaseCounter / ((double)actualPeriod - 1);

    if (!isAttenuated)
        envelope = 1 - envelope;

    if (!isContinued && loopCounter != 0)
        envelope = 0;

    if (isContinued)
    {
        if (isAlternated && (loopCounter & 1) == 1)
            envelope = 1 - envelope;

        if (isHold && loopCounter != 0)
            envelope = (isAttenuated != isAlternated) ? 1 : 0;
    }

    // update counters
    if (phaseCounter >= actualPeriod - 1)
    {
        loopCounter++;
    }
    phaseCounter = (phaseCounter + 1) % actualPeriod;

    // convert to internal volume
    return roundf(31 * fabs(envelope));
}
