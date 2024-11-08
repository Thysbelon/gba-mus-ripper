/*
 * This file is part of GBA Sound Riper
 * (c) 2012, 2014 Bregalad
 * This is free and open source software
 *
 * This class deals with internal representation of
 * GBA instruments and converts them to SF2 instruments
 */

#include "gba_instr.hpp"
#include <cmath>
#include <cstdio>
#include "hex_string.hpp"
#include <vector>
#include "sf2cute/include/sf2cute.hpp"
#include <utility>
extern FILE *inGBA;					// Related .gba file

void GBAInstr::addModulatorsToGlobalZone(SFInstrumentZone* global_instrument_zone, uint8_t otherMods){
	SFModulator modDepthIncUniLinMod(SFMidiController::kModulationDepth,
			SFControllerDirection::kIncrease, SFControllerPolarity::kUnipolar,
			SFControllerType::kLinear);
	SFModulator cc21IncUniConvMod(SFMidiController::kController21,
			SFControllerDirection::kIncrease, SFControllerPolarity::kUnipolar,
			SFControllerType::kConvex);
	SFModulator cc115DecUniConcMod(SFMidiController::kController115,
			SFControllerDirection::kDecrease, SFControllerPolarity::kUnipolar,
			SFControllerType::kConcave);
	SFModulator cc116DecUniSwiMod(SFMidiController::kController116,
			SFControllerDirection::kDecrease, SFControllerPolarity::kUnipolar,
			SFControllerType::kSwitch);
	SFModulator cc26IncUniConvMod(SFMidiController::kController26,
			SFControllerDirection::kIncrease, SFControllerPolarity::kUnipolar,
			SFControllerType::kConvex);
	
	int LFOspeedLFOSamount = 9572;
	int LFOspeedBPMamount = -10793;
	int LFOspeedAdjustAmount = -4887; // optimized for 35 LFOS and 116 BPM
	int LFOdelayAmount = 15000;
	
	// override the default pitch modulator for CC1 to prevent unnecessary vibrato where the original song had panpot or volume modulation. https://www.mail-archive.com/fluid-dev@nongnu.org/msg05330.html
	// disable default modDepth2VibLFOpitch modulator
	global_instrument_zone->SetModulator(SFModulatorItem(
			modDepthIncUniLinMod,
		SFGenerator::kVibLfoToPitch,
		0,
		SFModulator(0),
		SFTransform::kLinear));

	// GBA modDepth2ModLFOpitch modulator that only activates when CC110 is 0
	global_instrument_zone->SetModulator(SFModulatorItem(
			modDepthIncUniLinMod,
		SFGenerator::kModLfoToPitch,
		800,
		SFModulator(SFMidiController::kController110,
			SFControllerDirection::kDecrease, SFControllerPolarity::kUnipolar,
			SFControllerType::kSwitch),
		SFTransform::kLinear));

	// GBA modDepth2ModLFOvol modulator that only activates when CC111 is 127
	global_instrument_zone->SetModulator(SFModulatorItem(
			modDepthIncUniLinMod,
		SFGenerator::kModLfoToVolume,
		-200,
		SFModulator(SFMidiController::kController111,
			SFControllerDirection::kIncrease, SFControllerPolarity::kUnipolar,
			SFControllerType::kSwitch),
		SFTransform::kLinear));

	// GBA mod-speed modulator mod
	global_instrument_zone->SetModulator(SFModulatorItem(
			cc21IncUniConvMod,
		SFGenerator::kFreqModLFO,
		LFOspeedLFOSamount,
		SFModulator(0),
		SFTransform::kLinear));
	global_instrument_zone->SetModulator(SFModulatorItem(
			cc115DecUniConcMod,
		SFGenerator::kFreqModLFO,
		LFOspeedBPMamount,
		SFModulator(0),
		SFTransform::kLinear));
	global_instrument_zone->SetModulator(SFModulatorItem(
			cc116DecUniSwiMod,
		SFGenerator::kFreqModLFO,
		LFOspeedAdjustAmount,
		SFModulator(0),
		SFTransform::kLinear));

	// GBA mod-delay modulator mod // TODO: analyze MP2K LFO delay to make this more accurate
	global_instrument_zone->SetModulator(SFModulatorItem(
			cc26IncUniConvMod,
		SFGenerator::kDelayModLFO,
		LFOdelayAmount,
		SFModulator(0),
		SFTransform::kLinear));
	
	if ((otherMods & 0b00000001) == 0b00000001) {
		// volume modulator // lines up with lv almost perfectly EXCEPT for an MP2K VOL of 0
		global_instrument_zone->SetModulator(SFModulatorItem(
				SFModulator(SFMidiController::kChannelVolume,
				SFControllerDirection::kDecrease, SFControllerPolarity::kUnipolar,
				SFControllerType::kConcave),
			SFGenerator::kInitialAttenuation,
			480,
			SFModulator(0),
			SFTransform::kLinear));
	}
	if ((otherMods & 0b00000010) == 0b00000010) {
		// disble default reverb modulator 
		global_instrument_zone->SetModulator(SFModulatorItem(
				SFModulator(SFMidiController::kReverbSendLevel,
				SFControllerDirection::kIncrease, SFControllerPolarity::kUnipolar,
				SFControllerType::kLinear),
			SFGenerator::kReverbEffectsSend,
			0,
			SFModulator(0),
			SFTransform::kLinear));
		
		// new reverb modulator 1
		global_instrument_zone->SetModulator(SFModulatorItem(
				SFModulator(SFMidiController::kReverbSendLevel,
				SFControllerDirection::kIncrease, SFControllerPolarity::kUnipolar,
				SFControllerType::kConvex),
			SFGenerator::kReverbEffectsSend,
			200,
			SFModulator(0),
			SFTransform::kLinear));
		// new reverb modulator 2
		global_instrument_zone->SetModulator(SFModulatorItem(
				SFModulator(SFMidiController::kReverbSendLevel,
				SFControllerDirection::kDecrease, SFControllerPolarity::kUnipolar,
				SFControllerType::kLinear),
			SFGenerator::kReverbEffectsSend,
			-60,
			SFModulator(0),
			SFTransform::kLinear));
	}
}

bool operator <(const inst_data&i, const inst_data& j)
{
	if (j.word2 != i.word2) return i.word2 < j.word2;
	else if (j.word1 != i.word1) return i.word1 < j.word1;
	else return i.word0 < j.word0;
}

uint32_t GBAInstr::get_GBA_pointer()
{
	uint32_t p;
	fread(&p, 4, 1, inGBA);
	return p & 0x3FFFFFF;
}

void GBAInstr::generate_adsr_generators(const uint32_t adsr, SFInstrumentZone* instrument_zone /*needs to be a pointer or else instrument_zone will not be altered*/)
{
	// Get separate components
	int attack = adsr & 0xFF;
	int decay = (adsr>>8) & 0xFF;
	int sustain = (adsr>>16) & 0xFF;
	int release = adsr>>24;

	// Add generators for ADSR envelope if required
	if (attack != 0xFF)
	{
		// Compute attack time - the sound engine is called 60 times per second
		// and adds "attack" to envelope every time the engine is called
		double att_time = (256/60.0) / attack;
		double att = 1200 * log2(att_time);
		//sf2->add_new_inst_generator(SFGenerator::attackVolEnv, uint16_t(att));
		instrument_zone->SetGenerator(SFGeneratorItem(SFGenerator::kAttackVolEnv, /*may need to be changed to signed int*/uint16_t(att)));
	}

	if (sustain != 0xFF)
	{
		double sus;
		// Compute attenuation in cB if sustain is non-zero
		if (sustain != 0) sus = 100 * log(256.0/sustain);
		// Special case where attenuation is infinite -> use max value
		else sus = 1000;

		//sf2->add_new_inst_generator(SFGenerator::sustainVolEnv, uint16_t(sus));
		instrument_zone->SetGenerator(SFGeneratorItem(SFGenerator::kSustainVolEnv, uint16_t(sus)));

		double dec_time = (log(256.0) /(log(256)-log(decay)))/60.0;
		dec_time *= 10/log(256);
		double dec = 1200 * log2(dec_time);
		//sf2->add_new_inst_generator(SFGenerator::decayVolEnv, uint16_t(dec));
		instrument_zone->SetGenerator(SFGeneratorItem(SFGenerator::kDecayVolEnv, uint16_t(dec)));
	}

	if (release != 0x00)
	{
		double rel_time = (log(256.0)/(log(256)-log(release)))/60.0;
		double rel = 1200 * log2(rel_time);
		//sf2->add_new_inst_generator(SFGenerator::releaseVolEnv, uint16_t(rel));
		instrument_zone->SetGenerator(SFGeneratorItem(SFGenerator::kReleaseVolEnv, uint16_t(rel)));
	}
}

void GBAInstr::generate_psg_adsr_generators(const uint32_t adsr, SFInstrumentZone* instrument_zone)
{
	// Get separate components
	int attack = adsr & 0xFF;
	int decay = (adsr>>8) & 0xFF;
	int sustain = (adsr>>16) & 0xFF;
	int release = adsr>>24;

	// Reject instrument if invalid values !
	if (attack > 15 || decay > 15 || sustain > 15 || release > 15) throw -1;

	// Add generators for ADSR envelope if required
	if (attack != 0)
	{
		// Compute attack time - the sound engine is called 60 times per second
		// and adds "attack" to envelope every time the engine is called
		double att_time = attack/5.0;
		double att = 1200 * log2(att_time);
		//sf2->add_new_inst_generator(SFGenerator::attackVolEnv, uint16_t(att));
		instrument_zone->SetGenerator(SFGeneratorItem(SFGenerator::kAttackVolEnv, uint16_t(att)));
	}

	if (sustain != 15)
	{
		double sus;
		// Compute attenuation in cB if sustain is non-zero
		if (sustain != 0) sus = 100 * log(15.0/sustain);
		// Special case where attenuation is infinite -> use max value
		else sus = 1000;

		//sf2->add_new_inst_generator(SFGenerator::sustainVolEnv, uint16_t(sus));
		instrument_zone->SetGenerator(SFGeneratorItem(SFGenerator::kSustainVolEnv, uint16_t(sus)));

		double dec_time = decay/5.0;
		double dec = 1200 * log2(dec_time+1);
		//sf2->add_new_inst_generator(SFGenerator::decayVolEnv, uint16_t(dec));
		instrument_zone->SetGenerator(SFGeneratorItem(SFGenerator::kDecayVolEnv, uint16_t(dec)));
	}

	if (release != 0)
	{
		double rel_time = release/5.0;
		double rel = 1200 * log2(rel_time);
		//sf2->add_new_inst_generator(SFGenerator::releaseVolEnv, uint16_t(rel));
		instrument_zone->SetGenerator(SFGeneratorItem(SFGenerator::kReleaseVolEnv, uint16_t(rel)));
	}
}

// Build a SF2 instrument form a GBA sampled instrument
std::pair<int, std::shared_ptr<SFInstrument>> GBAInstr::build_sampled_instrument(const inst_data inst)
{
	// Do nothing if this instrument already exists !
	inst_it it = inst_map.find(inst);
	if (it != inst_map.end()) return (*it).second;

	// The flag is set if no scaling should be done if the instrument type is 8
	bool no_scale = (inst.word0&0xff) == 0x08;

	// Get sample pointer
	uint32_t sample_pointer = inst.word1 & 0x3ffffff;

	// Determine if loop is enabled (it's dumb but we have to seek just for this)
	if (fseek(inGBA, sample_pointer|3, SEEK_SET)) throw -1;
	bool loop_flag = fgetc(inGBA) == 0x40;

	// Build pointed sample
	//int sample_index = samples.build_sample(sample_pointer);
	std::pair< int, std::vector< std::shared_ptr<SFSample> > > sampleRetPair = samples.build_sample(sample_pointer);
	int sample_index = sampleRetPair.first;
	std::shared_ptr<SFSample> sampleSF2Pointer = sampleRetPair.second[0];

	// Instrument's name
	std::string name = "sample @0x" + hex(sample_pointer);

	// Create instrument bag
	//sf2->add_new_instrument(name.c_str());
	//sf2->add_new_inst_bag();
	SFInstrument new_instrument(name);
	SFInstrumentZone instrument_zone;
	// later: new_instrument.AddZone(instrument_zone); sf2->AddInstrument(new_instrument)

	// Add generator to prevent scaling if required
	//if (no_scale)
	//	sf2->add_new_inst_generator(SFGenerator::scaleTuning, 0);
	if (no_scale)
		instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kScaleTuning, 0));

	//generate_adsr_generators(inst.word2);
	generate_adsr_generators(inst.word2, &instrument_zone);
	//sf2->add_new_inst_generator(SFGenerator::sampleModes, loop_flag ? 1 : 0);
	instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, loop_flag ? 1 : 0));
	//sf2->add_new_inst_generator(SFGenerator::sampleID, sample_index);
	instrument_zone.set_sample(sampleSF2Pointer);
	instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample_index)); // Instead of creating the instrument with the sample, the instrument is created empty then the sampleID is set afterwards. (the sample has already been added to the soundfont). *However*, this doesn't work with sf2cute because it requires passing pointers. sf2.cpp probably also did pointer stuff internally.
	
	// set various modulators to more accurately match GBA output. TODO: make these -raw only?
	SFInstrumentZone global_instrument_zone;
	addModulatorsToGlobalZone(&global_instrument_zone, otherMods);
	new_instrument.set_global_zone(global_instrument_zone);
	
	// finish making instrument and insert into sf2 (I think the old library sf2.cpp did this automatically)
	new_instrument.AddZone(instrument_zone);
	std::shared_ptr<SFInstrument> shared_instrument = std::make_shared<SFInstrument>(new_instrument);
	sf2->AddInstrument(shared_instrument);

	// Add instrument to list
	//inst_map[inst] = cur_inst_index;
	inst_map[inst] = std::make_pair(cur_inst_index, shared_instrument);
	//return cur_inst_index ++;
	
	//std::pair<int, std::shared_ptr<SFInstrument>> retPair;
	//retPair.first = cur_inst_index ++;
	//retPair.second = shared_instrument;
	//return retPair;
	return std::make_pair(cur_inst_index ++, shared_instrument);
}

// Create new SF2 from every key split GBA instrument
std::pair<int, std::shared_ptr<SFInstrument>> GBAInstr::build_every_keysplit_instrument(const inst_data inst)
{
	// Do nothing if this instrument already exists !
	inst_it it = inst_map.find(inst);
	if (it != inst_map.end()) return (*it).second;

	//printf("GBAInstr default_sample_rate: %i\n", default_sample_rate);
	
	// I'm sorry for doing a dumb copy/pase of the routine right above
	// But there was too much differences to handles to practically handle it with flags
	// therefore I didn't really had a choice.
	uint32_t baseaddress = inst.word1 & 0x3ffffff;
	std::string name = "EveryKeySplit @0x" + hex(baseaddress);
	//sf2->add_new_instrument(name.c_str());
	SFInstrument new_instrument(name);
	
	// START LFO MODULATORS
	SFInstrumentZone global_instrument_zone;
	addModulatorsToGlobalZone(&global_instrument_zone, otherMods);
	new_instrument.set_global_zone(global_instrument_zone);	
	// END LFO MODULATORS

	// Loop through all keys
	for (int key = 0; key < 128; key ++)
	{
		try
		{
			// Seek at the key's instrument
			if (fseek(inGBA, baseaddress + 12*key, SEEK_SET)) throw -1;

			// Read instrument data
			int instrType = fgetc(inGBA);		// Instrument type
			int keynum = fgetc(inGBA);			// Key (every key split instrument only)
		/*  int unused_byte =*/ fgetc(inGBA);		// Unknown/unused byte
			int panning = fgetc(inGBA);			// Panning (every key split instrument only)

			// The flag is set if no scaling should be done on the sample
			bool no_scale = false;

			uint32_t main_word, adsr;
			fread(&main_word, 4, 1, inGBA);

			// Get ADSR envelope
			fread(&adsr, 4, 1, inGBA);

			int sample_index;
			std::pair< int, std::vector< std::shared_ptr<SFSample> > > sampleRetPair;
			std::shared_ptr<SFSample> sampleSF2Pointer;
			bool loop_flag = true;

			SFInstrumentZone instrument_zone; // variable must be in scope for those last few generators
			
			switch (instrType & 0x0f)
			{
				case 8:
					no_scale = true;
				case 0:
				{
					// Determine if loop is enabled and read sample's pitch
					uint32_t sample_pointer = main_word & 0x3ffffff;
					if (fseek(inGBA, sample_pointer|3, SEEK_SET)) throw -1;
					loop_flag = fgetc(inGBA) == 0x40;

					uint32_t pitch;
					fread(&pitch, 4, 1, inGBA);

					// Build pointed sample
					//sample_index = samples.build_sample(sample_pointer);
					sampleRetPair = samples.build_sample(sample_pointer);
					sample_index = sampleRetPair.first;
					sampleSF2Pointer = sampleRetPair.second[0];

					// Add a bag for this key
					//sf2->add_new_inst_bag();
					//SFInstrumentZone instrument_zone();
					//sf2->add_new_inst_generator(SFGenerator::keyRange, key, key);
					instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(key, key)));
					//generate_adsr_generators(adsr);
					generate_adsr_generators(adsr, &instrument_zone);
					// Add generator to prevent scaling if required
					if (no_scale)
						instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kScaleTuning, 0));

					// Compute base note and fine tune from pitch
					double delta_note = 12.0 * log2(default_sample_rate * 1024.0 / pitch);
					int rootkey = 60 + int(round(delta_note));

					// Override root key with the value we need
					//sf2->add_new_inst_generator(SFGenerator::overridingRootKey, rootkey - keynum + key);
					instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kOverridingRootKey, rootkey - keynum + key));
					
				}	break;

				case 4:
				case 12:
				{
					// Determine whenever the note is metallic noise, normal noise, or invalid
					bool metal_flag;
					if (main_word == 0x1000000)
						metal_flag = true;
					else if (main_word == 0)
						metal_flag = false;
					else
						throw -1;

					// Build corresponding sample
					//sample_index = samples.build_noise_sample(metal_flag, keynum);
					sampleRetPair = samples.build_noise_sample(metal_flag, keynum);
					sample_index = sampleRetPair.first;
					sampleSF2Pointer = sampleRetPair.second[0];
					//sf2->add_new_inst_bag();
					//SFInstrumentZone instrument_zone();
					//sf2->add_new_inst_generator(SFGenerator::keyRange, key, key);
					instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(key, key)));
					//generate_psg_adsr_generators(adsr);
					generate_psg_adsr_generators(adsr, &instrument_zone);
					//sf2->add_new_inst_generator(SFGenerator::overridingRootKey, key);
					instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kOverridingRootKey, key));
				}	break;

				// Ignore other kind of instruments
				default : throw -1;
			}

			//if (panning != 0)
			//	sf2->add_new_inst_generator(SFGenerator::pan, int((panning-192) * (500/128.0)));
			if (panning != 0)
				instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kPan, int((panning-192) * (500/128.0))));
			// Same as a normal sample
			//sf2->add_new_inst_generator(SFGenerator::sampleModes, loop_flag ? 1 : 0);
			instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, loop_flag ? 1 : 0));
			//sf2->add_new_inst_generator(SFGenerator::sampleID, sample_index);
			instrument_zone.set_sample(sampleSF2Pointer);
			instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample_index));
			
			// add instrument_zone to instrument (added with sf2cute)
			new_instrument.AddZone(instrument_zone);
		}
		catch (...) {}	// Continue to next key when there is a major problem
	}
	// Add instrument to list
	std::shared_ptr<SFInstrument> shared_instrument = std::make_shared<SFInstrument>(new_instrument);
	sf2->AddInstrument(shared_instrument);
	//inst_map[inst] = cur_inst_index;
	inst_map[inst] = std::make_pair(cur_inst_index, shared_instrument);
	//return cur_inst_index ++;
	std::pair<int, std::shared_ptr<SFInstrument>> retPair;
	retPair.first = cur_inst_index ++;
	retPair.second = shared_instrument;
	return retPair;
}

// Build a SF2 instrument from a GBA key split instrument
std::pair<int, std::shared_ptr<SFInstrument>> GBAInstr::build_keysplit_instrument(const inst_data inst)
{
	// Do nothing if this instrument already exists !
	inst_it it = inst_map.find(inst);
	if (it != inst_map.end()) return (*it).second;

	uint32_t base_pointer = inst.word1 & 0x3ffffff;
	uint32_t key_table = inst.word2 & 0x3ffffff;

	// Decode key-table in usable data
	std::vector<int8_t> split_list, index_list;

	int8_t key = 0;
	int prev_index = -1;
	int current_index;
	if (fseek(inGBA, key_table, SEEK_SET)) throw -1;

	// Add instrument to list
	std::string name = "0x" + hex(base_pointer) + " key split";
	//sf2->add_new_instrument(name.c_str());
	SFInstrument new_instrument(name);
	
	// START LFO MODULATORS
	SFInstrumentZone global_instrument_zone;
	addModulatorsToGlobalZone(&global_instrument_zone, otherMods);
	new_instrument.set_global_zone(global_instrument_zone);
	// END LFO MODULATORS

	do
	{
		int index = fgetc(inGBA);

		// Detect where there is changes in the index table
		current_index = index;
		if (prev_index != current_index)
		{
			split_list.push_back(key);
			index_list.push_back(current_index);
			prev_index = current_index;
		}
	}
	while (++key > 0);

	// Final entry for the last split
	split_list.push_back(0x80);

	for (unsigned int i=0; i<index_list.size(); i++)
	{
		try
		{
			// Seek to pointed instrument
			if (fseek(inGBA, base_pointer + 12*index_list[i], SEEK_SET)) throw -1;

			// Once again I'm sorry for the dumb copy/pase
			// but doing it all with flags would have been quite complex

			int inst_type = fgetc(inGBA);		// Instrument type
		 /* int keynum = */ fgetc(inGBA);		// Key (every key split instrument only)
		 /* int unused_byte = */ fgetc(inGBA);	// Unknown/unused byte
		 /* int panning = */ fgetc(inGBA);		// Panning (every key split instrument only)

			// The flag is set if no scaling should be done on the sample
			bool no_scale = inst_type==8;

			// Get sample pointer
			uint32_t sample_pointer;
			fread(&sample_pointer, 1, 4, inGBA);
			sample_pointer &= 0x3ffffff;

			// Get ADSR envelope
			uint32_t adsr;
			fread(&adsr, 4, 1, inGBA);

			// For now GameBoy instruments aren't supported
			// (I wonder if any game ever used this)
			if ((inst_type & 0x07) != 0) continue;

			// Determine if loop is enabled (it's dumb but we have to seek just for this)
			if (fseek(inGBA, sample_pointer|3, SEEK_SET)) throw -1;
			bool loop_flag = fgetc(inGBA) == 0x40;

			// Build pointed sample
			//int sample_index = samples.build_sample(sample_pointer);
			std::pair< uint32_t, std::vector< std::shared_ptr<SFSample> > > sampleRetPair = samples.build_sample(sample_pointer);
			int sample_index = sampleRetPair.first;
			std::shared_ptr<SFSample> sampleSF2Pointer = sampleRetPair.second[0];

			// Create instrument bag
			//sf2->add_new_inst_bag();
			SFInstrumentZone instrument_zone;

			// Particularity here : An additional bag to select the key range
			//sf2->add_new_inst_generator(SFGenerator::keyRange, split_list[i], split_list[i+1]-1);
			instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(split_list[i], split_list[i+1]-1)));

			// Add generator to prevent scaling if required
			//if (no_scale)
			//	sf2->add_new_inst_generator(SFGenerator::scaleTuning, 0);
			if (no_scale)
				instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kScaleTuning, 0));

			generate_adsr_generators(adsr, &instrument_zone);
			//sf2->add_new_inst_generator(SFGenerator::sampleModes, loop_flag ? 1 : 0);
			instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, loop_flag ? 1 : 0));
			//sf2->add_new_inst_generator(SFGenerator::sampleID, sample_index);
			instrument_zone.set_sample(sampleSF2Pointer);
			instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample_index));
			
			// add zone to instrument
			new_instrument.AddZone(instrument_zone);
		}
		catch (...) {}		// Silently continue to next key if anything bad happens // TODO: put warning message here.
	}
	std::shared_ptr<SFInstrument> shared_instrument = std::make_shared<SFInstrument>(new_instrument);
	sf2->AddInstrument(shared_instrument);
	//inst_map[inst] = cur_inst_index;
	inst_map[inst] = std::make_pair(cur_inst_index, shared_instrument);
	//return cur_inst_index ++;
	std::pair<int, std::shared_ptr<SFInstrument>> retPair;
	retPair.first = cur_inst_index ++;
	retPair.second = shared_instrument;
	return retPair;
}

// Build gameboy channel 3 instrument
std::pair<int, std::shared_ptr<SFInstrument>> GBAInstr::build_GB3_instrument(const inst_data inst)
{
	// Do nothing if this instrument already exists !
	inst_it it = inst_map.find(inst);
	if (it != inst_map.end()) return (*it).second;

	// Get sample pointer
	uint32_t sample_pointer = inst.word1 & 0x3ffffff;

	// Try to seek to see if the pointer is valid, if it's not then abort
	if (fseek(inGBA, sample_pointer, SEEK_SET)) throw -1;

	//int sample = samples.build_GB3_samples(sample_pointer);
	std::pair< int, std::vector< std::shared_ptr<SFSample> > > sampleRetPair = samples.build_GB3_samples(sample_pointer);
	int sample = sampleRetPair.first;
	std::vector< std::shared_ptr<SFSample> > sampleSF2PointerVector = sampleRetPair.second; // contains 4 samples

	std::string name = "GB3 @0x" + hex(sample_pointer);
	//sf2->add_new_instrument(name.c_str());
	SFInstrument new_instrument(name);

	// Global zone
	//sf2->add_new_inst_bag();
	SFInstrumentZone instrument_zone;
	generate_psg_adsr_generators(inst.word2, &instrument_zone); // looking at the global instrument zone in polyphone, there are no adsr settings, so this statement doesn't do anything? The same is true of both sf2.cpp and sf2cute.
	//new_instrument.AddZone(instrument_zone);
	addModulatorsToGlobalZone(&instrument_zone, otherMods);
	new_instrument.set_global_zone(instrument_zone);

	int myNum[8] = {0, 52, 53, 64, 65, 76, 77, 127};
	for (int i=0; i<4; i++){
		SFInstrumentZone instrument_zone_inner;
		instrument_zone_inner.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(myNum[i*2], myNum[i*2+1])));
		instrument_zone_inner.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, 1));
		instrument_zone_inner.set_sample(sampleSF2PointerVector[i]);
		instrument_zone_inner.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample-(3-i)));
		new_instrument.AddZone(instrument_zone_inner);
	}
	////sf2->add_new_inst_bag();
	//SFInstrumentZone instrument_zone1();
	////sf2->add_new_inst_generator(SFGenerator::keyRange, 0, 52);
	//instrument_zone1.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(0, 52)));
	////sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	//instrument_zone1.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, 1));
	////sf2->add_new_inst_generator(SFGenerator::sampleID, sample-3);
	//instrument_zone1.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample-3));
	//new_instrument.AddZone(instrument_zone1);
	////sf2->add_new_inst_bag();
	//SFInstrumentZone instrument_zone2();
	////sf2->add_new_inst_generator(SFGenerator::keyRange, 53, 64);
	//instrument_zone2.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(53, 64)));
	////sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	//instrument_zone2.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, 1));
	////sf2->add_new_inst_generator(SFGenerator::sampleID, sample-2);
	//instrument_zone2.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample-2));
	//new_instrument.AddZone(instrument_zone2);
	////sf2->add_new_inst_bag();
	//SFInstrumentZone instrument_zone3();
	////sf2->add_new_inst_generator(SFGenerator::keyRange, 65, 76);
	//instrument_zone3.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(65, 76)));
	////sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	//instrument_zone3.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, 1));
	////sf2->add_new_inst_generator(SFGenerator::sampleID, sample-1);
	//instrument_zone3.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample-1));
	//new_instrument.AddZone(instrument_zone3);
	////sf2->add_new_inst_bag();
	//SFInstrumentZone instrument_zone4();
	////sf2->add_new_inst_generator(SFGenerator::keyRange, 77, 127);
	//instrument_zone4.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(77, 127)));
	////sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	//instrument_zone4.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, 1));
	////sf2->add_new_inst_generator(SFGenerator::sampleID, sample);
	//instrument_zone4.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample));
	//new_instrument.AddZone(instrument_zone4);

	std::shared_ptr<SFInstrument> shared_instrument = std::make_shared<SFInstrument>(new_instrument);
	sf2->AddInstrument(shared_instrument);
	//inst_map[inst] = cur_inst_index;
	inst_map[inst] = std::make_pair(cur_inst_index, shared_instrument);
	//return cur_inst_index ++;
	std::pair<int, std::shared_ptr<SFInstrument>> retPair;
	retPair.first = cur_inst_index ++;
	retPair.second = shared_instrument;
	return retPair;
}

// Build GameBoy pulse wave instrument
std::pair<int, std::shared_ptr<SFInstrument>> GBAInstr::build_pulse_instrument(const inst_data inst)
{
	// Do nothing if this instrument already exists !
	inst_it it = inst_map.find(inst);
	if (it != inst_map.end()) return (*it).second;

	unsigned int duty_cycle = inst.word1;
	// The difference between 75% and 25% duty cycles is inaudible therefore
	// I simply replace 75% duty cycles by 25%
	if (duty_cycle == 3) duty_cycle = 1;
	if (duty_cycle > 3) throw -1;

	//int sample = samples.build_pulse_samples(duty_cycle);
	std::pair< int, std::vector< std::shared_ptr<SFSample> > > sampleRetPair = samples.build_pulse_samples(duty_cycle);
	int sample = sampleRetPair.first;
	std::vector< std::shared_ptr<SFSample> > sampleSF2PointerVector = sampleRetPair.second; // contains 5 samples
	std::string name = "pulse " + std::to_string(duty_cycle);
	//sf2->add_new_instrument(name.c_str());
	SFInstrument new_instrument(name);

	// Global zone
	//sf2->add_new_inst_bag();
	SFInstrumentZone instrument_zone;
	generate_psg_adsr_generators(inst.word2, &instrument_zone);
	addModulatorsToGlobalZone(&instrument_zone, otherMods);
	new_instrument.set_global_zone(instrument_zone);

	int myNum[10] = {0, 45, 46, 57, 58, 69, 70, 81, 82, 127};
	for (int i=0; i<5; i++){
		SFInstrumentZone instrument_zone_inner;
		instrument_zone_inner.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(myNum[i*2], myNum[i*2+1])));
		instrument_zone_inner.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, 1));
		instrument_zone_inner.set_sample(sampleSF2PointerVector[i]);
		instrument_zone_inner.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample-(4-i)));
		new_instrument.AddZone(instrument_zone_inner);
	}
	//sf2->add_new_inst_bag();
	//sf2->add_new_inst_generator(SFGenerator::keyRange, 0, 45);
	//sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	//sf2->add_new_inst_generator(SFGenerator::sampleID, sample-4);
	//sf2->add_new_inst_bag();
	//sf2->add_new_inst_generator(SFGenerator::keyRange, 46, 57);
	//sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	//sf2->add_new_inst_generator(SFGenerator::sampleID, sample-3);
	//sf2->add_new_inst_bag();
	//sf2->add_new_inst_generator(SFGenerator::keyRange, 58, 69);
	//sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	//sf2->add_new_inst_generator(SFGenerator::sampleID, sample-2);
	//sf2->add_new_inst_bag();
	//sf2->add_new_inst_generator(SFGenerator::keyRange, 70, 81);
	//sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	//sf2->add_new_inst_generator(SFGenerator::sampleID, sample-1);
	//sf2->add_new_inst_bag();
	//sf2->add_new_inst_generator(SFGenerator::keyRange, 82, 127);
	//sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	//sf2->add_new_inst_generator(SFGenerator::sampleID, sample);

	std::shared_ptr<SFInstrument> shared_instrument = std::make_shared<SFInstrument>(new_instrument);
	sf2->AddInstrument(shared_instrument);
	//inst_map[inst] = cur_inst_index;
	inst_map[inst] = std::make_pair(cur_inst_index, shared_instrument);
	//return cur_inst_index ++;
	std::pair<int, std::shared_ptr<SFInstrument>> retPair;
	retPair.first = cur_inst_index ++;
	retPair.second = shared_instrument;
	return retPair;
}

// Build GameBoy white noise instrument
std::pair<int, std::shared_ptr<SFInstrument>> GBAInstr::build_noise_instrument(const inst_data inst)
{
	// Do nothing if this instrument already exists !
	inst_it it = inst_map.find(inst);
	if (it != inst_map.end()) return (*it).second;

	// 0 = normal, 1 = metallic, anything else = invalid
	if (inst.word1 > 1) throw -1;
	bool metallic = inst.word1;

	std::string name = metallic ? "GB metallic noise" : "GB noise";
	//sf2->add_new_instrument(name.c_str());
	SFInstrument new_instrument(name);

	// Global zone
	//sf2->add_new_inst_bag();
	//generate_psg_adsr_generators(inst.word2);
	SFInstrumentZone instrument_zone;
	generate_psg_adsr_generators(inst.word2, &instrument_zone);
	addModulatorsToGlobalZone(&instrument_zone, otherMods);
	new_instrument.set_global_zone(instrument_zone);

	//sf2->add_new_inst_bag();
	SFInstrumentZone instrument_zone2;
	//int sample42 = samples.build_noise_sample(metallic, 42);
	std::pair< int, std::vector< std::shared_ptr<SFSample> > > sampleRetPair42 = samples.build_noise_sample(metallic, 42);
	int sample42 = sampleRetPair42.first;
	std::shared_ptr<SFSample> sampleSF2Pointer42 = sampleRetPair42.second[0];
	//sf2->add_new_inst_generator(SFGenerator::keyRange, 0, 42);
	instrument_zone2.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(0, 42)));
	//sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	instrument_zone2.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, 1));
	//sf2->add_new_inst_generator(SFGenerator::sampleID, sample42);
	instrument_zone2.set_sample(sampleSF2Pointer42);
	instrument_zone2.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample42));
	new_instrument.AddZone(instrument_zone2);

	for (int key = 43; key <=77; key++)
	{
		//sf2->add_new_inst_bag();
		SFInstrumentZone instrument_zone_key;
		//int sample = samples.build_noise_sample(metallic, key);
		std::pair< int, std::vector< std::shared_ptr<SFSample> > > sampleRetPair_key = samples.build_noise_sample(metallic, key);
		int sample = sampleRetPair_key.first;
		std::shared_ptr<SFSample> sampleSF2Pointer_key = sampleRetPair_key.second[0];
		//sf2->add_new_inst_generator(SFGenerator::keyRange, key, key);
		instrument_zone_key.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(key, key)));
		//sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
		instrument_zone_key.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, 1));
		//sf2->add_new_inst_generator(SFGenerator::sampleID, sample);
		instrument_zone_key.set_sample(sampleSF2Pointer_key);
		instrument_zone_key.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample));
		new_instrument.AddZone(instrument_zone_key);
	}

	//sf2->add_new_inst_bag();
	SFInstrumentZone instrument_zone3;
	//int sample78 = samples.build_noise_sample(metallic, 78);
	std::pair< int, std::vector< std::shared_ptr<SFSample> > > sampleRetPair78 = samples.build_noise_sample(metallic, 78);
	int sample78 = sampleRetPair78.first;
	std::shared_ptr<SFSample> sampleSF2Pointer78 = sampleRetPair78.second[0];
	//sf2->add_new_inst_generator(SFGenerator::keyRange, 78, 127);
	instrument_zone3.SetGenerator(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(78, 127)));
	//sf2->add_new_inst_generator(SFGenerator::sampleModes, 1);
	instrument_zone3.SetGenerator(SFGeneratorItem(SFGenerator::kSampleModes, 1));
	//sf2->add_new_inst_generator(SFGenerator::scaleTuning, 0);
	instrument_zone3.SetGenerator(SFGeneratorItem(SFGenerator::kScaleTuning, 0));
	//sf2->add_new_inst_generator(SFGenerator::sampleID, sample78);
	instrument_zone3.set_sample(sampleSF2Pointer78);
	instrument_zone3.SetGenerator(SFGeneratorItem(SFGenerator::kSampleID, sample78));
	new_instrument.AddZone(instrument_zone3);

	std::shared_ptr<SFInstrument> shared_instrument = std::make_shared<SFInstrument>(new_instrument);
	sf2->AddInstrument(shared_instrument);
	//inst_map[inst] = cur_inst_index;
	inst_map[inst] = std::make_pair(cur_inst_index, shared_instrument);
	//return cur_inst_index ++;
	std::pair<int, std::shared_ptr<SFInstrument>> retPair;
	retPair.first = cur_inst_index ++;
	retPair.second = shared_instrument;
	return retPair;
}
