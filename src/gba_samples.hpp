/*
 * Sound Font Samples class
 *
 * This program is part of GBA SoundFontRiper (c) 2012 by Bregalad
 * This is free and open source software.
 */

#pragma once

//#include "sf2.hpp"
#include "sf2cute/include/sf2cute.hpp"
#include <vector>
#include <cstdio>
#include <cstring>
#include <utility> // for pair
//#include <tuple>

typedef enum
{
	UNSIGNED_8,
	SIGNED_8,
	SIGNED_16,
	GAMEBOY_CH3,
	BDPCM
}
SampleType;

using namespace sf2cute;

class GBASamples
{	// List of pointers to samples within the .gba file, position is # of sample in .sf2
	//std::vector<uint32_t> samples_list;
	//std::vector<std::tuple<uint32_t, std::shared_ptr<SFSample>, std::shared_ptr<SFSample>, std::shared_ptr<SFSample>, std::shared_ptr<SFSample>>> samples_list; // ?
	std::vector< std::pair< uint32_t, std::vector< std::shared_ptr<SFSample> > > > samples_list; // all because GB3 makes multiple samples, while everything else makes one.
	// Related sf2 class
	SoundFont *sf2; //SF2 *sf2;
	unsigned int default_sample_rate; // sf2cute doesn't have a feature to define a default sample rate for the soundfont, so have the sample_rate as a property of this class instead.
	std::shared_ptr<SFSample> gbaToNewSample(/*GBA specific params*/ FILE *sampleFile /*usually inGBA, sometimes psg_data or goldensun_synth*/, SampleType inputFmt, uint32_t pointer, uint32_t size, /*sf2 params*/ std::string name, uint32_t loop_pos/*start_loop*/, uint32_t original_pitch/*root key?*/, uint32_t pitch_correction/*microtuning?*/, uint32_t sample_rate);
	std::shared_ptr<SFSample> gbaToNewSample(/*GBA specific params*/ FILE *sampleFile /*usually inGBA, sometimes psg_data or goldensun_synth*/, SampleType inputFmt, uint32_t pointer, uint32_t size, /*sf2 params*/ std::string name, uint32_t loop_pos/*start_loop*/, uint32_t original_pitch/*root key?*/, uint32_t pitch_correction/*microtuning?*/); // Add new sample using default sample rate

public:
	GBASamples(/*SF2 *sf2*/ SoundFont *sf2, unsigned int sample_rate) : sf2(sf2), default_sample_rate(sample_rate)
	{}

	// Convert a normal sample to SoundFont format
	std::pair< uint32_t, std::vector< std::shared_ptr<SFSample> > > build_sample(uint32_t pointer);
	// Convert a Game Boy channel 3 sample to SoundFont format
	std::pair< uint32_t, std::vector< std::shared_ptr<SFSample> > > build_GB3_samples(uint32_t pointer);
	// Convert a Game Boy pulse (channels 1, 2) sample
	std::pair< uint32_t, std::vector< std::shared_ptr<SFSample> > > build_pulse_samples(unsigned int duty_cycle);
	// Convert a Game Boy noise (channel 4) sample
	std::pair< uint32_t, std::vector< std::shared_ptr<SFSample> > > build_noise_sample(bool metallic, int key);
};
