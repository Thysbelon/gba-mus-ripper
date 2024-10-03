/*
 * This file is part of GBA Sound Riper
 * (c) 2012, 2014 Bregalad
 * This is free and open source software
 *
 * This class deals with internal representation of
 * GBA samples and converts them to SF2 samples
 */

#include "gba_samples.hpp"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include "hex_string.hpp"
#include <vector> // I think I need to include this because I'm declaring vector variables in this file.
#include "sf2cute/include/sf2cute.hpp"
#include <utility> // for pair
//#include <tuple>

extern FILE *inGBA;
extern FILE *psg_data;
extern FILE *goldensun_synth;

std::shared_ptr<SFSample> GBASamples::gbaToNewSample(/*GBA specific params*/ FILE *sampleFile /*usually inGBA, sometimes psg_data or goldensun_synth*/, SampleType inputFmt, uint32_t pointer, uint32_t size, /*sf2 params*/ std::string name, uint32_t loop_pos/*start_loop*/, uint32_t original_pitch/*root key?*/, uint32_t pitch_correction/*microtuning?*/, uint32_t sample_rate){
	// sample data is obtained from the gba file using "pointer" and "size"
	// GBA sample data can be a few different formats. sf2 sample data must be SIGNED_16. sf2.cpp converted all formats to SIGNED_16 automatically.
	
	// from sf2_chunks.hpp
	// Seek at the start of the sample in input file
	fseek(sampleFile, pointer, SEEK_SET);
	
	int16_t *outbuf = new int16_t[size];
	
	switch (inputFmt)
	{
		// Source is unsigned 8 bits
		case UNSIGNED_8:
		{
			uint8_t *data = new uint8_t[size];
			fread(data, 1, size, sampleFile);
			// Convert to signed 16 bits
			for (unsigned int j=0; j < size; j++)
				outbuf[j] = (data[j] - 0x80) << 8;
			delete[] data;
		}	break;

		// Source is signed 8 bits
		case SIGNED_8:
		{
			int8_t *data = new int8_t[size];
			fread(data, 1, size, sampleFile);

			for (unsigned int j=0; j < size; j++)
				outbuf[j] = data[j] << 8;
			delete[] data;
		}	break;

		case SIGNED_16:
			// Just read raw data, no conversion needed
			fread(outbuf, 2, size, sampleFile);
			break;

		case GAMEBOY_CH3:
		{
			// Conversion lookup table
			const int16_t conv_tbl[] =
			{
				-0x4000, -0x3800, -0x3000, -0x2800, -0x2000, -0x1800, -0x0100, -0x0800,
				0x0000, 0x0800, 0x1000, 0x1800, 0x2000, 0x2800, 0x3000, 0x3800
			};

			int num_of_repts = size/32;
			// Data is always on 16 bytes
			uint8_t data[16];
			fread(data, 1, 16, sampleFile);

			for (int j=0, l=0; j<16; j++)
			{
				for (int k=num_of_repts; k!=0; k--, l++)
					outbuf[l] = conv_tbl[data[j]>>4];

				for (int k=num_of_repts; k!=0; k--, l++)
					outbuf[l] = conv_tbl[data[j]&0xf];
			}
		}	break;

		case BDPCM:
		{
			static const int8_t delta_lut[] = {0, 1, 4, 9, 16, 25, 36, 49, -64, -49, -36, -25, -16, -9, -4, -1};

			/*
			 * A block consists of an initial signed 8 bit PCM byte
			 * followed by 63 nibbles stored in 32 bytes.
			 * The first of these bytes has a zero padded (unused) high nibble.
			 * This makes up of a total block size of 65 (0x21) bytes each.
			 *
			 * Decoding works like this:
			 * The initial byte can be directly read without decoding. Then each
			 * next sample can be decoded by putting the nibble into the delta-lookup-table
			 * and adding that value to the previously calculated sample
			 * until the end of the block is reached.
			 */

			unsigned int nblocks = size / 64;		// 64 samples per block

			char (*data)[33] = new char[nblocks][33];
			fread(data, 33, nblocks, sampleFile);

			for (unsigned int block=0; block < nblocks; ++block)
			{
				int8_t sample = data[block][0];
				outbuf[64*block] = sample << 8;
				sample += delta_lut[data[block][1] & 0xf];
				outbuf[64*block+1] = sample << 8;
				for (unsigned int j = 1; j < 32; ++j)
				{
					uint8_t d = data[block][j+1];
					sample += delta_lut[d >> 4];
					outbuf[64*block+2*j] = sample << 8;
					sample += delta_lut[d & 0xf];
					outbuf[64*block+2*j+1]= sample << 8;
				}
			}
			memset(outbuf+64*nblocks, 0, size-64*nblocks);		// Remaining samples are always 0

			delete[] data;
		}   break;
	}
	
	// https://stackoverflow.com/questions/8777603/what-is-the-simplest-way-to-convert-array-to-vector
	std::vector<int16_t> sample_data(outbuf, outbuf + size);
	std::shared_ptr<SFSample> shared_sample = sf2->NewSample(
		name,
		sample_data,
		loop_pos,
		uint32_t(sample_data.size()),
		sample_rate,
		uint8_t(original_pitch), // I think this is a key number
		int8_t(pitch_correction) // I think this is cents
	);
	
	return shared_sample;
	
	/*
	int16_t temp_array[size];
	fread();
	// https://stackoverflow.com/questions/8777603/what-is-the-simplest-way-to-convert-array-to-vector
	std::vector<int16_t> sample_data;
	sf2->NewSample(name)
	*/
}

// Add new sample using default sample rate
std::shared_ptr<SFSample> GBASamples::gbaToNewSample(/*GBA specific params*/ FILE *sampleFile /*usually inGBA, sometimes psg_data or goldensun_synth*/, SampleType inputFmt, uint32_t pointer, uint32_t size, /*sf2 params*/ std::string name, uint32_t loop_pos/*start_loop*/, uint32_t original_pitch/*root key?*/, uint32_t pitch_correction/*microtuning?*/){
	return gbaToNewSample(sampleFile, inputFmt, pointer, size, name, loop_pos, original_pitch, pitch_correction, default_sample_rate);
}

std::pair< uint32_t, std::vector< std::shared_ptr<SFSample> > > GBASamples::build_sample(uint32_t pointer)
{	// Do nothing if sample already exists
	for (int i=samples_list.size()-1; i >= 0; --i)
		if (samples_list[i].first == pointer) return std::make_pair(i, samples_list[i].second);

	std::shared_ptr<SFSample> sampleSF2Pointer;
	// Read sample data
	if (fseek(inGBA, pointer, SEEK_SET)) throw -1;

	struct
	{
		uint32_t loop;
		uint32_t pitch;
		uint32_t loop_pos;
		uint32_t len;
	}
	hdr;
	fread(&hdr, 4, 4, inGBA);

	//Now we should make sure the data is coherent, and reject
	//the samples if errors are suspected

	//Detect invalid samples
	bool loop_en;
	bool bdpcm_en = false;

	if (hdr.loop == 0x40000000)
		loop_en = true;
	else if (hdr.loop == 0x00000000)
		loop_en = false;
	else if (hdr.loop == 0x1)
	{
		bdpcm_en = true;    // Detect compressed samples
		loop_en = false;
	}
	else
		throw -1;			// Invalid loop -> return error

	// Compute SF2 base note and fine tune from GBA pitch
	// GBA pitch is 1024 * Mid_C frequency
	double delta_note = 12 * log2(default_sample_rate * 1024.0 / hdr.pitch);
	double int_delta_note = round(delta_note);
	unsigned int pitch_correction = int((int_delta_note - delta_note) * 100);
	unsigned int original_pitch = 60 + (int)int_delta_note;

	// Detect Golden Sun samples
	if (goldensun_synth && hdr.len == 0 && hdr.loop_pos == 0)
	{
		if (fgetc(inGBA) != 0x80) throw -1;
		uint8_t type = fgetc(inGBA);
		switch (type)
		{
			case 0:		// Square wave
			{
				std::string name = "Square @0x" + hex(pointer);
				uint8_t duty_cycle = fgetc(inGBA);
				uint8_t change_speed = fgetc(inGBA);
				if (change_speed == 0)
				{	// Square wave with constant duty cycle
					unsigned int base_pointer = 128 + 64 * (duty_cycle >> 2);
					//sf2->add_new_sample(goldensun_synth, UNSIGNED_8, name.c_str(), base_pointer, 64, true, 0, original_pitch, pitch_correction); // !
					sampleSF2Pointer = gbaToNewSample(goldensun_synth, UNSIGNED_8, base_pointer, 64, name, 0, original_pitch, pitch_correction);
				}
				else
				{	// Sqaure wave with variable duty cycle, not exact, but sounds close enough
					//sf2->add_new_sample(goldensun_synth, UNSIGNED_8, name.c_str(), 128, 8192, true, 0, original_pitch, pitch_correction);
					sampleSF2Pointer=gbaToNewSample(goldensun_synth, UNSIGNED_8, 128, 8192, name, 0, original_pitch, pitch_correction);
				}
			}	break;

			case 1:		// Saw wave
			{
				std::string name = "Saw @0x" + hex(pointer);
				//sf2->add_new_sample(goldensun_synth, UNSIGNED_8, name.c_str(), 0, 64, true, 0, original_pitch, pitch_correction);
				sampleSF2Pointer=gbaToNewSample(goldensun_synth, UNSIGNED_8, 0, 64, name, 0, original_pitch, pitch_correction);
			}	break;

			case 2:		// Triangle wave
			{
				std::string name = "Triangle @0x" + hex(pointer);
				//sf2->add_new_sample(goldensun_synth, UNSIGNED_8, name.c_str(), 64, 64, true, 0, original_pitch, pitch_correction);
				sampleSF2Pointer=gbaToNewSample(goldensun_synth, UNSIGNED_8, 64, 64, name, 0, original_pitch, pitch_correction);
			}	break;

			default :
				throw -1;
		}
	}
	else
	{
		//Prevent samples which are way too long or too short
		if (hdr.len < 16 || hdr.len > 0x3FFFFF) throw -1;

		//Prevent samples with illegal loop point from happening
		if (hdr.loop_pos > hdr.len-8)
		{
			puts("Warning : Illegal loop point detected\n");
			hdr.loop_pos = 0;
		}

		// Create (poetic) instrument name
		std::string name = (bdpcm_en ? "BDPCM @0x" : "Sample @0x") + hex(pointer);

		// Add the sample to output
		//sf2->add_new_sample(inGBA, bdpcm_en ? BDPCM : SIGNED_8, name.c_str(), pointer + 16, hdr.len, loop_en, hdr.loop_pos, original_pitch, pitch_correction);
		sampleSF2Pointer = gbaToNewSample(inGBA, bdpcm_en ? BDPCM : SIGNED_8, pointer + 16, hdr.len, name, hdr.loop_pos, original_pitch, pitch_correction);
	}
	std::vector<std::shared_ptr<SFSample>> retSampVec({sampleSF2Pointer});
	samples_list.push_back(std::make_pair(pointer, retSampVec));
	return std::make_pair(samples_list.size() - 1, retSampVec);
}

//Build game boy channel 3 sample
std::pair< uint32_t, std::vector< std::shared_ptr<SFSample> > > GBASamples::build_GB3_samples(uint32_t pointer)
{
	// Do nothing if sample already exists
	for (int i=samples_list.size()-1; i >= 0; --i)
		if (samples_list[i].first == pointer) return std::make_pair(i, samples_list[i].second);

	std::string name = "GB3 @0x" + hex(pointer);

	//sf2->add_new_sample(inGBA, GAMEBOY_CH3, (name + 'A').c_str(), pointer, 256, true, 0, 53, 24, 22050);
	std::shared_ptr<SFSample> sampleSF2Pointer1 = gbaToNewSample(inGBA, GAMEBOY_CH3, pointer, 256, name+'A', 0, 53, 24, 22050);
	//sf2->add_new_sample(inGBA, GAMEBOY_CH3, (name + 'B').c_str(), pointer, 128, true, 0, 65, 24, 22050);
	std::shared_ptr<SFSample> sampleSF2Pointer2 = gbaToNewSample(inGBA, GAMEBOY_CH3, pointer, 128, name+'B', 0, 65, 24, 22050);
	//sf2->add_new_sample(inGBA, GAMEBOY_CH3, (name + 'C').c_str(), pointer, 64, true, 0, 77, 24, 22050);
	std::shared_ptr<SFSample> sampleSF2Pointer3 = gbaToNewSample(inGBA, GAMEBOY_CH3, pointer, 64, name+'C', 0, 77, 24, 22050);
	//sf2->add_new_sample(inGBA, GAMEBOY_CH3, (name + 'D').c_str(), pointer, 32, true, 0, 89, 24, 22050);
	std::shared_ptr<SFSample> sampleSF2Pointer4 = gbaToNewSample(inGBA, GAMEBOY_CH3, pointer, 32, name+'D', 0, 89, 24, 22050);

	// We have to to add multiple entries to have the size of the list in sync
	// with the numeric indexes of samples....
	std::vector<std::shared_ptr<SFSample>> retSampVec({sampleSF2Pointer1, sampleSF2Pointer2, sampleSF2Pointer3, sampleSF2Pointer4});
	for (int i=4; i!=0; --i) samples_list.push_back(std::make_pair(pointer, retSampVec));
	return std::make_pair(samples_list.size() - 1, retSampVec);
}

//Build square wave sample
std::pair< uint32_t, std::vector< std::shared_ptr<SFSample> > > GBASamples::build_pulse_samples(unsigned int duty_cycle)
{	// Do nothing if sample already exists
	for (int i=samples_list.size()-1; i >= 0; --i)
		if (samples_list[i].first == duty_cycle) return std::make_pair(i, samples_list[i].second);

	std::shared_ptr<SFSample> sampleSF2Pointer;
	std::string name = "square ";
	switch (duty_cycle)
	{
		case 0:
			name += "12.5%";
			break;

		default:
			name += "25%";
			break;

		case 2:
			name += "50%";
			break;
	}

	//This data is referenced to my set of recordings
	//stored in "psg_data.raw"
	const int pointer_tbl[3][5] =
	{
			{0x0000, 0x2166, 0x3c88, 0x4bd2, 0x698a},
			{0x7798, 0x903e, 0xa15e, 0xb12c, 0xbf4a},
			{0xc958, 0xe200, 0xf4ec, 0x10534, 0x11360}
	};

	const int size_tbl[3][5] =
	{
			{0x10b3, 0xd91, 0x7a5, 0xdec, 0x707},
			{0xc53, 0x890, 0x7e7, 0x70f, 0x507},
			{0xc54, 0x976, 0x824, 0x716, 0x36b}
	};

	const int loop_size[5] = {689, 344, 172, 86, 43};

	std::vector<std::shared_ptr<SFSample>> retSampVec;
	for (int i = 0; i < 5; i++)
	{
		//sf2->add_new_sample(psg_data, SIGNED_16, (name + char('A' + i)).c_str(), pointer_tbl[duty_cycle][i], size_tbl[duty_cycle][i], true, size_tbl[duty_cycle][i]-loop_size[i], 36 + 12 * i, 38, 44100);
		retSampVec.push_back( gbaToNewSample(psg_data, SIGNED_16, pointer_tbl[duty_cycle][i], size_tbl[duty_cycle][i], name + char('A' + i), size_tbl[duty_cycle][i]-loop_size[i], 36 + 12 * i, 38, 44100) );
	}
	for (int i = 0; i < 5; i++)
	{
		samples_list.push_back(std::make_pair(duty_cycle, retSampVec));
	}
	return std::make_pair(samples_list.size()-1, retSampVec);
}

//Build white noise sample
std::pair< uint32_t, std::vector< std::shared_ptr<SFSample> > > GBASamples::build_noise_sample(bool metallic, int key)
{
	//prevent out of range keys
	if (key < 42) key = 42;
	if (key > 77) key = 76;

	unsigned int num = metallic ? 3 + (key-42) : 80 + (key-42);

	// Do nothing if sample already exists
	//for (int i=samples_list.size()-1; i >= 0; --i)
	//	if (samples_list[i] == num) return i;
	for (int i=samples_list.size()-1; i >= 0; --i)
		if (samples_list[i].first == num) return std::make_pair(i, samples_list[i].second);

	std::string name = std::string("Noise ") + std::string(metallic ? "metallic " : "normal ") + std::to_string(key);

	const int pointer_tbl[] =
	{
		72246, 160446, 248646, 336846, 425046, 513246, 601446, 689646, 777846,
		866046, 954246, 1042446, 1130646, 1218846, 1307046, 1395246, 1483446, 1571646, 1659846,
		1748046, 1836246, 1924446, 2012646, 2100846, 2189046, 2277246, 2387493, 2475690, 2552863,
		2619011, 2674134, 2718233, 2756819, 2789893, 2817455, 2839504, 2856041, 2867066, 2872578
	};

	const int normal_len_tbl[] =
	{
		88200, 88200, 88200, 88200, 88200, 88200, 88200, 88200, 88200, 88200,
		88200, 88200, 88200, 88200, 88200, 88200, 88200, 88200, 88200, 88200, 88200, 88200, 88200,
		88200, 88200, 110247, 88197, 77173, 66148, 55123, 44099, 38586, 33074, 27562, 22049, 16537,
		11025, 5512, 2756
	};

	const int metallic_len_tbl[] =
	{	43755, 38286, 32817, 27347, 21878, 19143, 16408, 13674, 10939, 9572,
		8204, 6837, 5469, 4786, 4102, 3418, 2735, 2393, 2051, 1709, 1367, 1196, 1026, 855, 684,
		598, 513,	427, 342, 299, 256, 214, 171, 150, 128, 107, 85, 64
	};

	//sf2->add_new_sample(psg_data, UNSIGNED_8, name.c_str(), pointer_tbl[key-42], metallic ? metallic_len_tbl[key-42] : normal_len_tbl[key-42], true, 0, key, 0, 44100);
	std::shared_ptr<SFSample> sampleSF2Pointer = gbaToNewSample(psg_data, UNSIGNED_8, pointer_tbl[key-42], metallic ? metallic_len_tbl[key-42] : normal_len_tbl[key-42], name, 0, key, 0, 44100);

	std::vector<std::shared_ptr<SFSample>> retSampVec({sampleSF2Pointer});
	samples_list.push_back(std::make_pair(num, retSampVec));
	return std::make_pair(samples_list.size() - 1, retSampVec);
}
