/*
  ==============================================================================

	FemtoSynthSound.h
	Created: 7 Jul 2020 10:01:20pm
	Author:  moge3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FemtoSynthSound : public juce::SynthesiserSound
{
public:
	FemtoSynthSound();
	~FemtoSynthSound();
	
	bool appliesToNote(int midiNoteNum) override { return true; }
	bool appliesToChannel(int midiChannel) override { return true; }
};
