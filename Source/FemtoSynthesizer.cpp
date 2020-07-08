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

FemtoSynthesizer::FemtoSynthesizer(juce::AudioProcessor& p)
{
	clearVoices();
	addVoice(new FemtoSynthVoice());

	clearSounds();
	addSound(new FemtoSynthSound());
}

FemtoSynthesizer::~FemtoSynthesizer()
{
}
