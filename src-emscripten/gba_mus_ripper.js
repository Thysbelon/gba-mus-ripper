// NOTE: this is unfinished. I may scrap this, and instead make a slight copy of gba_mus_ripper.cpp that includes all the other main functions in its own code.

/*
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
static uint32_t song_tbl_ptr = 0;

static const int sample_rates[] = {-1, 5734, 7884, 10512, 13379, 15768, 18157, 21024, 26758, 31536, 36314, 40137, 42048};

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
		"[address]: Force address of the song table manually. This is required for manually dumping music data from ROMs where the location can't be detected automatically.\n"
	);
	exit(0);
}
*/

async function get_GBA_pointer(/*file*/ inGBA, startPos=0){
	let p=0;
	//fread(&p, 4, 1, inGBA);
	//p = new Uint32Array(await inGBA.slice(0,4).arrayBuffer())[0];
	p = await fread(4, 1, inGBA, startPos)[0];
	return p - 0x8000000;
}

//  Convert number to string with always 4 digits (even if leading zeroes)
//  Mother 3 is a game which needs the 4 digits.
/*string*/ function dec4(/*number*/ inNum){
	return inNum.toString().padStart(4, '0');
}

/*
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
*/

/*number*/ async function fread(sizeOfEachElemInBytes, numOfElems, inFile, startPos=0){ // substitute for cpp fread.
	const myArrayBuffer=await inFile.slice(startPos,startPos+numOfElems*sizeOfEachElemInBytes).arrayBuffer();
	var myTypeArray;
	switch (sizeOfEachElemInBytes){
		case 1:
			myTypeArray=newUint8Array(myArrayBuffer);
			break;
		case 2:
			myTypeArray=newUint16Array(myArrayBuffer);
			break;
		case 4:
			myTypeArray=newUint32Array(myArrayBuffer);
			break;
	}
	return myTypeArray;
}

/*number*/ async function fgetc(inFile, startPos=0){ // substitute for cpp fgetc.
	const myArrayBuffer=await inFile.slice(startPos,startPos+1).arrayBuffer();
	var myTypeArray;
	myTypeArray=newUint8Array(myArrayBuffer);
	return myTypeArray[0];
}

async function main(/*file*/inGBA, /*object*/opt){
	var curFilePointer=0;
	var inGBA_size=0;
	var song_tbl_ptr=0;
	const sample_rates = [-1, 5734, 7884, 10512, 13379, 15768, 18157, 21024, 26758, 31536, 36314, 40137, 42048];
	
	// Parse arguments
	{
		const defaultOpt = {
			gm: false,
			rc: true,
			xg: false,
			sb: false,
			raw: false,
			address: undefined, // if set, is a number
		}
		if (opt==undefined) {
			opt = defaultOpt;
		} else {
			for (option in defaultOpt){
				if (opt?.[option]==undefined){
					opt[option]=defaultOpt[option];
				}
			}
		}
	}
	if (typeof opt.address == 'number') {song_tbl_ptr=opt.address}
	
	var sample_rate = 0, main_volume = 0;		// Use default values when those are '0'
	
	// If the user hasn't provided an address manually, we'll try to automatically detect it
	if (!song_tbl_ptr){
		// Auto-detect address of sappy engine
		{
			const module={
				noInitialRun: true,
				arguments: ["-r", "8000", "-L", "-e"],
				preRun: () => {
					module.FS.writeFile("input.gba", new Uint8Array(await inGBA.arrayBuffer()));
				},
			}
			const sappy_detector_inst=await sappy_detector(module);
			var sound_engine_adr = sappy_detector_inst.run();
		}
		
		// Exit if no sappy engine was found
		if (!sound_engine_adr) {/*exit(0)*/ throw new Error('No MP2K engine found. You will need to input the address of the sound engine in opt'); return};
		
		console.debug(sound_engine_adr); // current startPos for fread
		curFilePointer=sound_engine_adr;
		
		// Engine parameter's word
		const parameter_word = await fread(4, 1, inGBA, curFilePointer)[0]; // cpp fread advances the file pointer by the total amount of bytes read. https://cplusplus.com/reference/cstdio/fread/
		curFilePointer+=4;
		
		// Get sampling rate
		sample_rate = sample_rates[(parameter_word >> 16) & 0xf];
		main_volume = (parameter_word >> 12) & 0xf;
		
		// Compute address of song table
		var song_levels=0;			// Read # of song levels
		song_levels = await fread(4, 1, inGBA, curFilePointer)[0];
		curFilePointer+=4;
		console.info(`# of song levels: ${song_levels}`);
		song_tbl_ptr = await get_GBA_pointer(inGBA, curFilePointer) + 12 * song_levels;
		curFilePointer+=4;
	}
	
	// Create a zip file named like the input ROM
	// TODO
	
	//  Get the size of the input GBA file
	inGBA_size = inGBA.size; // TODO: make sure this size is the same as cpp original
	
	if (song_tbl_ptr >= inGBA_size){
		throw new Error('Fatal error: Song table at '+song_tbl_ptr+' is past the end of the file.');
		return;
	}
	
	console.info("Parsing song table...");
	// New list of songs
	const song_list=[];
	// New list of sound banks
	const sound_bank_list=new Set();
	
	console.debug(song_tbl_ptr); // current startPos for fread
	curFilePointer=song_tbl_ptr;
	
	// Ignores entries which are made of 0s at the start of the song table
	// this fix was necessarily for the game Fire Emblem
	var song_pointer=0;
	while (true)
	{
		song_pointer = await fread(4, 1, inGBA, curFilePointer)[0];
		curFilePointer+=4;
		if (song_pointer != 0) {break;}
		song_tbl_ptr += 4;
	}
	
	{
		let i = 0;
		while (true)
		{
			song_pointer -= 0x8000000;		// Adjust pointer

			// Stop as soon as we met with an invalid pointer
			if (song_pointer == 0 || song_pointer >= inGBA_size) {break};

			for (let j = 4; j != 0; --j) {await fgetc(inGBA, curFilePointer); curFilePointer+=1}		// Discard 4 bytes (sound group)
			song_list.push(song_pointer);			// Add pointer to list
			i++;
			song_pointer = await fread(4, 1, inGBA, curFilePointer)[0];
			curFilePointer+=4;
		};
		// As soon as data that is not a valid pointer is found, the song table is terminated
		
		// End of song table
		var song_tbl_end_ptr = 8*i + song_tbl_ptr;
	}

	console.info("Collecting sound bank list...");
	
	//typedef std::set<uint32_t>::iterator bank_t;
	//bank_t *sound_bank_index_list = new bank_t[song_list.size()];
	const sound_bank_index_list=Array(song_list.length).fill('nothing') // a list that is the same length as the song_list. When paired with the song_list, the sound_bank_index_list will tell you which sound bank each song uses. 
	//For example, an RPG game could contain two sound banks, one used for the overworld, and one used for battle. Thus the game contains two sound banks total. the sound_bank_index_list would tell you which songs use the overworld sound bank, and which songs use the battle sound bank.

	for (let i = 0, l=song_list.length; i < l; i++){
		// Ignore unused song, which points to the end of the song table (for some reason)
		if (song_list[i] != song_tbl_end_ptr){
			// Seek to song data
			//if (fseek(inGBA, song_list[i] + 4, SEEK_SET)) continue;
			curFilePointer=song_list[i]+4; 
			const sound_bank_ptr = await get_GBA_pointer(inGBA, curFilePointer);
			curFilePointer+=4;

			// Add sound bank to list if not already in the list
			//sound_bank_index_list[i] = sound_bank_list.insert(sound_bank_ptr).first;
			sound_bank_list.add(sound_bank_ptr);
			sound_bank_index_list[i]=sound_bank_ptr; // JS sets don't have a convenient way to use indexes or pointers to access set items.
		}
	}
	
	// Prepare for using Song Ripper
	
	// Create directories for each sound bank if separate banks is enabled
	if (sb) // TODO: have this make directories in the zip file that will be downloaded
	{
		for (bank_t j = sound_bank_list.begin(); j != sound_bank_list.end(); ++j)
		{
			unsigned int d = std::distance(sound_bank_list.begin(), j);
			std::string subdir = outPath + '/' + "soundbank_" + dec4(d);
			mkdir(subdir);
		}
	}
	
	for (let i = 0, l=song_list.length; i < l; i++){
		if (song_list[i] != song_tbl_end_ptr){
			const bank_index = Array.from(sound_bank_list).findIndex((x) => x === sound_bank_index_list[i]);
			//std::string seq_rip_cmd = prg_prefix + "song_ripper \"" + inGBA_path + "\" \"" + outPath;
			let seq_rip_cmd = ['input.gba','output.gba'];

			// Add leading zeroes to file name
			if (sb) {seq_rip_cmd += "/soundbank_" + dec4(bank_index);}
			seq_rip_cmd += "/song" + dec4(i) + ".mid\"";

			seq_rip_cmd += " 0x" + hex(song_list[i]);
			seq_rip_cmd += rc ? " -rc" : (xg ? " -xg": " -gs");
			if (!raw)
			{
				seq_rip_cmd += " -sv";
				seq_rip_cmd += " -lv";
			}
			// Bank number, if banks are not separated
			if (!sb)
				seq_rip_cmd += " -b" + std::to_string(bank_index);

			printf("Song %u\n", i);

			printf("DEBUG: Going to call system(%s)\n", seq_rip_cmd.c_str());
			if (!system(seq_rip_cmd.c_str())) puts("An error occurred while calling song_ripper.");
		}
	}
	delete[] sound_bank_index_list;
}