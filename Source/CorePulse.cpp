/*
  ==============================================================================

	CorePulse.cpp
	Created: 7 Jul 2020 11:29:50pm
	Author:  moge3

  ==============================================================================
*/

#include "CorePulse.h"
#define _USE_MATH_DEFINES
#include <cmath>

double CorePulse::renderNextSample(int32_t period)
{
    int32_t actualPeriod = period;
    if (actualPeriod < 1)
        actualPeriod = 1;
    if (actualPeriod > 0xfff)
        actualPeriod = 0xfff;

    counter = (counter + 1) % (actualPeriod * 2);
    double signal = (counter == 0) ? 1 : (counter == actualPeriod) ? -1 : 0;

    previousSample = signal + 0.999 * previousSample;
    return previousSample;
}
