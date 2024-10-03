/*
 * Sound Font Instruments class
 *
 * This program is part of GBA SoundFontRiper (c) 2012, 2014 by Bregalad
 * This is free and open source software.
 */

#pragma once

#include <cstdint>
#include <map>
//#include "sf2.hpp"
#include "sf2cute/include/sf2cute.hpp"
#include <utility>
#include "gba_samples.hpp"

using namespace sf2cute;

struct inst_data
{
	uint32_t word0;
	uint32_t word1;
	uint32_t word2;
};

//typedef std::map<inst_data, int>::iterator inst_it;
typedef std::map<inst_data, std::pair<int, std::shared_ptr<SFInstrument>>>::iterator inst_it;

class GBAInstr
{
// private:
	int cur_inst_index;
	unsigned int default_sample_rate; // sf2cute doesn't have a feature to define a default sample rate for the soundfont, so have the sample_rate as a property of this class instead.
	//std::map<inst_data, int> inst_map;	// Contains pointers to instruments within GBA file, their position is the # of instrument in the SF2
	std::map<inst_data, std::pair<int, std::shared_ptr<SFInstrument>>> inst_map;
	SoundFont *sf2; //SF2 *sf2;										// Related .sf2 file
	GBASamples samples;								// Related samples class

	// Convert pointer from GBA memory map to ROM memory map
	uint32_t get_GBA_pointer();
	// Apply ADSR envelope on the instrument
	void generate_adsr_generators(const uint32_t adsr, SFInstrumentZone* instrument_zone); //void generate_adsr_generators(const uint32_t adsr);
	void generate_psg_adsr_generators(const uint32_t adsr, SFInstrumentZone* instrument_zone); //void generate_psg_adsr_generators(const uint32_t adsr);

public:
	GBAInstr(/*SF2 *sf2*/ SoundFont *sf2, unsigned int sample_rate=22050) : cur_inst_index(0), sf2(sf2), samples(sf2, sample_rate), default_sample_rate(sample_rate) // NEEDS TESTING
	{}

	//Build a SF2 instrument form a GBA sampled instrument
	std::pair<int, std::shared_ptr<SFInstrument>> build_sampled_instrument(const inst_data inst);

	//Create new SF2 from every key split GBA instrument
	std::pair<int, std::shared_ptr<SFInstrument>> build_every_keysplit_instrument(const inst_data inst);

	//Build a SF2 instrument from a GBA key split instrument
	std::pair<int, std::shared_ptr<SFInstrument>> build_keysplit_instrument(const inst_data inst);

	//Build gameboy channel 3 instrument
	std::pair<int, std::shared_ptr<SFInstrument>> build_GB3_instrument(const inst_data inst);

	//Build GameBoy pulse wave instrument
	std::pair<int, std::shared_ptr<SFInstrument>> build_pulse_instrument(const inst_data inst);

	//Build GameBoy white noise instrument
	std::pair<int, std::shared_ptr<SFInstrument>> build_noise_instrument(const inst_data inst);
};
