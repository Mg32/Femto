/*
  ==============================================================================

	FemtoSynthesizer.h
	Created: 7 Jul 2020 10:02:06pm
	Author:  moge3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FemtoSynthesizer : public juce::Synthesiser
{
public:
	FemtoSynthesizer(juce::AudioProcessorValueTreeState& parameters);
	~FemtoSynthesizer();
};
