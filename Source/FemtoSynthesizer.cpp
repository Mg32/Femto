/*
  ==============================================================================

	FemtoSynthesizer.cpp
	Created: 7 Jul 2020 10:02:06pm
	Author:  moge3

  ==============================================================================
*/

#include "FemtoSynthesizer.h"
#include "FemtoSynthSound.h"
#include "FemtoSynthVoice.h"

FemtoSynthesizer::FemtoSynthesizer(juce::AudioProcessorValueTreeState& parameters)
{
	clearVoices();
	addVoice(new FemtoSynthVoice(parameters));

	clearSounds();
	addSound(new FemtoSynthSound());
}

FemtoSynthesizer::~FemtoSynthesizer()
{
}
