/**
 * GBAMusRipper (c) 2012-2015 Bregalad, (c) 2017-2018 CaptainSwag101
 * This is free and open source software
 *
 * This program analyze a Game Boy Advance ROM and search for a sound engine
 * named "Sappy" which is used in ~90% of commercial GBA games.
 *
 * If the engine is found it rips all musics to MIDI (.mid) format and all
 * instruments to SoundFont 2.0 (.sf2) format.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <set>
#include "hex_string.hpp"
#include <stdio.h> // popen
#include <string> // stoul
#include <utility> // pair
#include "yaml-cpp/include/yaml-cpp/yaml.h"

//#ifndef WIN32
//namespace sappy_detector
//{
//    #include "sappy_detector.c"             // The main::function is called directly on Linux
//}
//#endif

static FILE *inGBA;
static std::string inGBA_path;
static std::string outPath;
static size_t inGBA_size;
static std::string name;
static bool gm = false;
static bool xg = false;
static bool rc = false;
static bool sb = false;
static bool raw = false;
static bool dry = false;
static uint32_t song_tbl_ptr = 0;

static const int sample_rates[] = {-1, 5734 /*0 MP2K.yaml number*/, 7884 /*1*/, 10512 /*2*/, 13379, 15768, 18157, 21024, 26758, 31536, 36314, 40137, 42048};

static void print_instructions()
{
	puts(
		"  /==========================================================================\\\n"
		"-<   GBA Mus Ripper 3.3 (c) 2012-2015 Bregalad, (c) 2017-2018 CaptainSwag101  >-\n"
		"  \\==========================================================================/\n\n"
		"Usage: gba_mus_ripper (input_file) [-o output_directory] [address] [flags]\n\n"
		"-gm  : Give General MIDI names to presets. Note that this will only change the names and will NOT magically turn the soundfont into a General MIDI compliant soundfont.\n"
		"-rc  : Rearrange channels in output MIDIs so channel 10 is avoided. Needed by sound cards where it's impossible to disable \"drums\" on channel 10 even with GS or XG commands.\n"
		"-xg  : Output MIDI will be compliant to XG standard (instead of default GS standard).\n"
		"-sb  : Separate banks. Every sound bank is riper to a different .sf2 file and placed into different sub-folders (instead of doing it in a single .sf2 file and a single folder).\n"
		"-raw : Output MIDIs exactly as they're encoded in ROM, without linearise volume and velocities and without simulating vibratos.\n"
		"-dry : Run the program without writing any files. Useful for developers testing the program.\n"
		"[address]: Force address of the song table manually. This is required for manually dumping music data from ROMs where the location can't be detected automatically.\n"
	);
	exit(0);
}

static uint32_t get_GBA_pointer()
{
	uint32_t p;
	fread(&p, 4, 1, inGBA);
	return p - 0x8000000;
}

static void mkdir(std::string name)
{
    #ifdef _WIN32
    system(("md \"" + name + "\"").c_str());
    #else
    system(("mkdir -p \"" + name + "\"").c_str());
    #endif
}

//  Convert number to string with always 4 digits (even if leading zeroes)
//  Mother 3 is a game which needs the 4 digits.
static std::string dec4(unsigned int n)
{
	std::string s;
	s += "0123456789"[n / 1000 % 10];
	s += "0123456789"[n / 100 % 10];
	s += "0123456789"[n / 10 % 10];
	s += "0123456789"[n % 10];
	return s;
}

/*inline*/ bool exists_test1 (const std::string& name) { // https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exists-using-standard-c-c11-14-17-c
	if (FILE *file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	} else {
		return false;
	}   
}

static uint32_t getSoundTable(std::string prg_prefix, std::string inGBA_path){
	#ifdef WIN32
	std::string mp2ktoolCmd = "\"\"" + prg_prefix + "mp2ktool\" songtable \"" + inGBA_path + "\"\"";
	#else
	std::string mp2ktoolCmd = prg_prefix + "mp2ktool songtable \"" + inGBA_path + "\"";
	#endif
	printf("DEBUG: going to call popen(%s)\n", mp2ktoolCmd.c_str());
	FILE *mp2ktoolFile = popen(mp2ktoolCmd.c_str(), "r"); // https://stackoverflow.com/questions/125828/capturing-stdout-from-a-system-command-optimally

	if (!mp2ktoolFile){
		return 0;
	}

	char buffer[1024];
	char *line = fgets(buffer, sizeof(buffer), mp2ktoolFile);
	uint32_t x = std::stoul(line, nullptr, 16); // https://stackoverflow.com/questions/1070497/c-convert-hex-string-to-signed-integer
	printf("result: 0x%08x\n", x); // https://stackoverflow.com/questions/14733761/printf-formatting-for-hexadecimal
	pclose(mp2ktoolFile);
	return x;
}

static std::pair<int, int> testSampleRateAndVolValidity(uint32_t paramAddress){ // adapted from sappy_detector.c
	const uint8_t testNums[] = {16/*for most games*/, 32/*for pokemon/gamefreak*/};
	for (int i=0; i<2; i++){
		fseek(inGBA, paramAddress - testNums[i], SEEK_SET);
		printf("testing with %d\n", testNums[i]);
		
		uint32_t parameter_word;
		fread(&parameter_word, 4, 1, inGBA);
		//printf("parameter_word: %lu\n", parameter_word);
		printf("parameter_word: 0x%x\n", parameter_word);
		
		int polyphony, mainVol, sampleRateIndex, dacBits;
		polyphony = (parameter_word & 0x000F00) >> 8;
		mainVol = (parameter_word & 0x00F000) >> 12;
		sampleRateIndex = (parameter_word & 0x0F0000) >> 16;
		dacBits = 17-((parameter_word & 0xF00000) >> 20);
		
		bool paramsValid = mainVol != 0
			&& polyphony <= 12
			&& dacBits >= 6
			&& dacBits <= 9
			&& sampleRateIndex >= 1
			&& sampleRateIndex <= 12
			&&((parameter_word & 0xff000000) == 0);
		
		if (paramsValid == true) return std::make_pair(sampleRateIndex, mainVol);
	}
	/* If neither is found there is an error */
	puts("Only a partial sound engine was found.");
	return std::make_pair(-1, -1);
}

static std::pair<int, int> getSampleRateIndexAndVol(std::string prg_prefix, std::string inGBA_path){
	std::string mp2ktoolCmd = prg_prefix + "mp2ktool info \"" + inGBA_path + "\"";
	printf("DEBUG: going to call popen(%s)\n", mp2ktoolCmd.c_str());
	FILE *mp2ktoolOutput = popen(mp2ktoolCmd.c_str(), "r");

	if (!mp2ktoolOutput){
		return std::make_pair(0,0);
	}

	char buffer[1024];
	for (int i=0; i<6; i++){ // skip 6 lines
		fgets(buffer, sizeof(buffer), mp2ktoolOutput);
	}
	char *line = fgets(buffer, sizeof(buffer), mp2ktoolOutput);
	//printf("line: %s\n", line);
	std::string stringLine(line);
	// "m4a_main                "
	stringLine.erase(0,24);
	//printf("stringLine after erase: %s[END OF STRING]\n", stringLine.c_str());
	if (stringLine=="null\n"){
		printf("address of sound engine could not be found.\n");
		pclose(mp2ktoolOutput);
		//return std::make_pair(0,0);
		
		fseek(inGBA, 0xAC, SEEK_SET);
		char gameCode[5];
		fread(&gameCode, 4, 1, inGBA);
		printf("gameCode: %s\n", gameCode);
		fseek(inGBA, 0xBC, SEEK_SET);
		uint8_t version;
		fread(&version, 1, 1, inGBA);
		printf("version: %i\n", version);
		// yaml reading here
		YAML::Node gameDatabase;
		std::string yamlFilePath="undefined";
		std::string possible_paths[]={".","./data","../data"};
		for (int i=0; i<3; i++){
			if (exists_test1(possible_paths[i]+"/MP2K.yaml")) {
				yamlFilePath=possible_paths[i];
				break;
			}
		}
		if (yamlFilePath=="undefined"){
			printf("warning: MP2K.yaml not found");
			return std::make_pair(0,0);
		}
		gameDatabase = YAML::LoadFile(yamlFilePath+"/MP2K.yaml");
		std::string stringGameCode(gameCode);
		YAML::Node curGameEntry;
		char zeroPaddedString[3];
		snprintf(zeroPaddedString, 3, "%02i", version);
		std::string testString = stringGameCode+"_"+std::string(zeroPaddedString);
		printf("testString: %s\n", testString.c_str());
		if (gameDatabase[testString]){
			curGameEntry = gameDatabase[testString]; 
		} else {
			return std::make_pair(0,0);
		}
		// check if the current entry is a copy of another entry
		if (curGameEntry["Copy"]) curGameEntry = gameDatabase[curGameEntry["Copy"].as<std::string>()];
		return std::make_pair(curGameEntry["SampleRate"].as<int>() + 1, curGameEntry["Volume"].as<int>());
	}
	uint32_t sound_engine_adr = std::stoul(stringLine, nullptr, 16); // https://stackoverflow.com/questions/1070497/c-convert-hex-string-to-signed-integer
	printf("sound_engine_adr: 0x%08x\n", sound_engine_adr); // https://stackoverflow.com/questions/14733761/printf-formatting-for-hexadecimal
	pclose(mp2ktoolOutput);
	
	/* Test validity of engine offset with -16 and -32 */
	std::pair<int, int> sampleRateIndexAndVol = testSampleRateAndVolValidity(sound_engine_adr);
	if (sampleRateIndexAndVol.first == -1) return std::make_pair(0,0);
  
	return sampleRateIndexAndVol;
}

static void parse_args(const int argc, char *const args[])
{
	if (argc < 1) print_instructions();

	bool path_found = false, song_tbl_found = false;
	for (int i = 0; i < argc; i++)
	{
		if (args[i][0] == '-')
		{
			if (!strcmp(args[i], "--help"))
				print_instructions();
			else if (!strcmp(args[i], "-gm"))
				gm = true;
			else if (!strcmp(args[i], "-xg"))
				xg = true;
			else if (!strcmp(args[i], "-rc"))
				rc = true;
			else if (!strcmp(args[i], "-sb"))
				sb = true;
			else if (!strcmp(args[i], "-raw"))
				raw = true;
			else if (!strcmp(args[i], "-dry"))
				dry = true;
            else if (!strcmp(args[i], "-o") && argc >= i + 1)
            {
                outPath = args[++i];
            }
			else
			{
				fprintf(stderr, "Error: Unknown command line option: %s. Try with --help to get information.\n", args[i]);
				exit(-1);
			}
		}
		// Convert given address to binary, use it instead of automatically detected one
		else if (!path_found)
		{
			// Get GBA file
			inGBA = fopen(args[i], "rb");
			if (!inGBA)
			{
				fprintf(stderr, "Error: Can't open file %s for reading.\n", args[i]);
				exit(-1);
			}

			// Name is filename without the extention and without path
			inGBA_path = std::string(args[i]);
			size_t separator_index = inGBA_path.find_last_of("/\\") + 1;
			name = inGBA_path.substr(separator_index, inGBA_path.find_last_of('.') - separator_index);

			// Path where the input GBA file is located
			outPath = inGBA_path.substr(0, separator_index) + '.';
			path_found = true;
		}
		else if (!song_tbl_found)
		{
			errno = 0;
			song_tbl_ptr = strtoul(args[i], 0, 0);
			if (errno)
			{
				fprintf(stderr, "Error: %s is not a valid song table address.\n", args[i]);
				exit(-1);
			}
			song_tbl_found = true;
		}
		else
		{
			fprintf(stderr, "Error: Don't know what to do with %s. Try with --help to get more information.\n", args[i]);
			exit(-1);
		}
	}
	if (!path_found)
	{
		fputs("Error: No input GBA file. Try with --help to get more information.\n", stderr);
		exit(-1);
	}
}

int main(int argc, char *const argv[])
{
	// Parse arguments (without program name)
	parse_args(argc - 1, argv + 1);

	// Compute program prefix (should be "", "./", "../" or whatever)
	std::string prg_name = argv[0];
	std::string prg_prefix = prg_name.substr(0, prg_name.rfind("gba_mus_ripper"));

	int sample_rate = 0, main_volume = 0;		// Use default values when those are '0'

	// If the user hasn't provided an address manually, we'll try to automatically detect it
	if (!song_tbl_ptr)
	{
		song_tbl_ptr = getSoundTable(prg_prefix, inGBA_path);
		if (!song_tbl_ptr) {
			fprintf(stderr, "Error: no MP2K song table was found.\n");
			exit(0);
		}
		
		// Auto-detect address of sappy engine
//#ifdef WIN32
//		// On windows, just use the 32-bit return code of the sappy_detector executable
//		std::string sappy_detector_cmd = "\"\"" + prg_prefix + "sappy_detector\" \"" + inGBA_path + "\"\""; // https://stackoverflow.com/questions/27975969/how-to-run-an-executable-with-spaces-using-stdsystem-on-windows
//		printf("DEBUG: Going to call system(%s)\n", sappy_detector_cmd.c_str());
//		int sound_engine_adr = std::system(sappy_detector_cmd.c_str());
//#else
//		// On linux the function is duplicated in this executable
//		const char *sappy_detector_argv1 = inGBA_path.c_str();
//		int sound_engine_adr = sappy_detector::main(2, &sappy_detector_argv1 - 1);
//#endif

		//// Exit if no sappy engine was found
		//if (!sound_engine_adr) {
		//	fprintf(stderr, "Error: no MP2K engine was found.\n");
		//	exit(0);
		//}
    //
		//if (fseek(inGBA, sound_engine_adr, SEEK_SET))
		//{
		//	fprintf(stderr, "Error: Invalid offset within input GBA file: 0x%x\n", sound_engine_adr);
		//	exit(0);
		//}
    //
		//// Engine parameter's word
		//uint32_t parameter_word;
		//fread(&parameter_word, 4, 1, inGBA);
    //
		//// Get sampling rate
		//sample_rate = sample_rates[(parameter_word >> 16) & 0xf];
		//main_volume = (parameter_word >> 12) & 0xf;
    //
		//// Compute address of song table
		//uint32_t song_levels;			// Read # of song levels
		//fread(&song_levels, 4, 1, inGBA);
		//printf("# of song levels: %d\n", song_levels);
		//song_tbl_ptr = get_GBA_pointer() + 12 * song_levels;
	}
	
	// attempt to get sample rate
	std::pair<int, int> sampleRateIndexAndVol = getSampleRateIndexAndVol(prg_prefix, inGBA_path);
	sample_rate = sampleRateIndexAndVol.first != 0 ? sample_rates[sampleRateIndexAndVol.first] : 0;
	main_volume = sampleRateIndexAndVol.second;

	// Create a directory named like the input ROM, without the .gba extension
	mkdir(outPath);

	//  Get the size of the input GBA file
	fseek(inGBA, 0L, SEEK_END);
	inGBA_size = ftell(inGBA);

	if (song_tbl_ptr >= inGBA_size)
	{
		fprintf(stderr, "Fatal error: Song table at 0x%x is past the end of the file.\n", song_tbl_ptr);
		exit(-2);
	}

	printf("Parsing song table...");
	// New list of songs
	std::vector<uint32_t> song_list;
	// New list of sound banks
	std::set<uint32_t> sound_bank_list;

	if (fseek(inGBA, song_tbl_ptr, SEEK_SET))
	{
		fprintf(stderr, "Fatal error: Can't seek to song table at: 0x%x\n", song_tbl_ptr);
		exit(-3);
	}

	// Ignores entries which are made of 0s at the start of the song table
	// this fix was necessarily for the game Fire Emblem
	uint32_t song_pointer;
	while (true)
	{
		fread(&song_pointer, 4, 1, inGBA);
		if (song_pointer != 0) break;
		song_tbl_ptr += 4;
	}

	unsigned int i = 0;
	while (true)
	{
		song_pointer -= 0x8000000;		// Adjust pointer

		// Stop as soon as we met with an invalid pointer
		if (song_pointer == 0 || song_pointer >= inGBA_size) break;

		for (int j = 4; j != 0; --j) fgetc(inGBA);		// Discard 4 bytes (sound group)
		song_list.push_back(song_pointer);			// Add pointer to list
		i++;
		fread(&song_pointer, 4, 1, inGBA);
	};
	// As soon as data that is not a valid pointer is found, the song table is terminated

	// End of song table
	uint32_t song_tbl_end_ptr = 8*i + song_tbl_ptr;

	puts("Collecting sound bank list...");

	typedef std::set<uint32_t>::iterator bank_t;
	bank_t *sound_bank_index_list = new bank_t[song_list.size()];

	for (i = 0; i < song_list.size(); i++)
	{
		// Ignore unused song, which points to the end of the song table (for some reason)
		if (song_list[i] != song_tbl_end_ptr)
		{
			// Seek to song data
			if (fseek(inGBA, song_list[i] + 4, SEEK_SET)) continue;
			uint32_t sound_bank_ptr = get_GBA_pointer();

			// Add sound bank to list if not already in the list
			sound_bank_index_list[i] = sound_bank_list.insert(sound_bank_ptr).first;
		}
	}

	// Close GBA file so that SongRipper can access it
	fclose(inGBA);

	// Create directories for each sound bank if separate banks is enabled
	if (sb)
	{
		for (bank_t j = sound_bank_list.begin(); j != sound_bank_list.end(); ++j)
		{
			unsigned int d = std::distance(sound_bank_list.begin(), j);
			std::string subdir = outPath + '/' + "soundbank_" + dec4(d); // when going through the sound table, a song that uses sound group 4 could appear before a song that uses sound group 2. GBA_Mus_Ripper always gets the correct sound bank number because C++ sets automatically order their elements from smallest to largest; sound bank 2 should have a smaller pointer value than sound bank 4, so it will be in the correct order in a set.
			mkdir(subdir);
		}
	}

	for (i = 0; i < song_list.size(); i++)
	{
		if (song_list[i] != song_tbl_end_ptr)
		{
			unsigned int bank_index = distance(sound_bank_list.begin(), sound_bank_index_list[i]);
			#ifdef WIN32
			// I don't know if the extra quotes cause problems on Linux.
			std::string seq_rip_cmd = "\"\"" + prg_prefix + "song_ripper\" \"" + inGBA_path + "\" \"" + outPath;
			#else
			std::string seq_rip_cmd = prg_prefix + "song_ripper \"" + inGBA_path + "\" \"" + outPath;
			#endif

			// Add leading zeroes to file name
			if (sb) seq_rip_cmd += "/soundbank_" + dec4(bank_index);
			seq_rip_cmd += "/song" + dec4(i) + ".mid\"";

			seq_rip_cmd += " 0x" + hex(song_list[i]);
			seq_rip_cmd += rc ? " -rc" : (xg ? " -xg": " -gs");
			if (!raw)
			{
				//seq_rip_cmd += " -sv";
				seq_rip_cmd += " -lv";
			}
			// Bank number, if banks are not separated
			if (!sb)
				seq_rip_cmd += " -b" + std::to_string(bank_index);
			if (dry) seq_rip_cmd += " -dry";

			#ifdef WIN32
			seq_rip_cmd += "\"";
			#endif
			
			printf("Song %u\n", i);

			printf("DEBUG: Going to call system(%s)\n", seq_rip_cmd.c_str());
			if (!system(seq_rip_cmd.c_str())) puts("An error occurred while calling song_ripper.");
		}
	}
	delete[] sound_bank_index_list;

	if (sb)
	{
		// Rips each sound bank in a different file/folder
		for (bank_t j = sound_bank_list.begin(); j != sound_bank_list.end(); ++j)
		{
			unsigned int bank_index = distance(sound_bank_list.begin(), j);

			std::string sbnumber = dec4(bank_index);
			std::string foldername = "soundbank_" + sbnumber;
			#ifdef WIN32
			std::string sf_rip_args = "\"\"" + prg_prefix + "sound_font_ripper\" \"" + inGBA_path + "\" \"" + outPath + '/';
			#else
			std::string sf_rip_args = prg_prefix + "sound_font_ripper \"" + inGBA_path + "\" \"" + outPath + '/';
			#endif
			sf_rip_args += foldername + '/' + foldername /* + "_@" + hex(*j) */ + ".sf2\"";

			if (sample_rate) sf_rip_args += " -s" + std::to_string(sample_rate);
			if (main_volume)	sf_rip_args += " -mv" + std::to_string(main_volume);
			if (gm) sf_rip_args += " -gm";
			if (raw) sf_rip_args += " -atmod -revmod";
			if (dry) sf_rip_args += " -dry";
			sf_rip_args += " 0x" + hex(*j);
			
			#ifdef WIN32
			sf_rip_args += "\"";
			#endif

            printf("DEBUG: Goint to call system(%s)\n", sf_rip_args.c_str());
			system(sf_rip_args.c_str());
		}
	}
	else
	{
		// Rips each sound bank in a single soundfont file
		// Build argument list to call sound_font_riper
		// Output sound font named after the input ROM
		#ifdef WIN32
		std::string sf_rip_args = "\"\"" + prg_prefix + "sound_font_ripper\" \"" + inGBA_path + "\" \"" + outPath + '/' + name + ".sf2\"";
		#else
		std::string sf_rip_args = prg_prefix + "sound_font_ripper \"" + inGBA_path + "\" \"" + outPath + '/' + name + ".sf2\"";
		#endif
		if (sample_rate) sf_rip_args += " -s" + std::to_string(sample_rate);
		if (main_volume) sf_rip_args += " -mv" + std::to_string(main_volume);
		// Pass -gm argument if necessary
		if (gm) sf_rip_args += " -gm";
		if (raw) sf_rip_args += " -atmod -revmod";
		if (dry) sf_rip_args += " -dry";

		// Make sound banks addresses list.
		for (bank_t j = sound_bank_list.begin(); j != sound_bank_list.end(); ++j)
			sf_rip_args += " 0x" + hex(*j);
		
		#ifdef WIN32
		sf_rip_args += "\"";
		#endif
		
		// Call sound font ripper
        printf("DEBUG: Going to call system(%s)\n", sf_rip_args.c_str());
		system(sf_rip_args.c_str());
	}

	puts("Rip completed!");
	return 0;
}
