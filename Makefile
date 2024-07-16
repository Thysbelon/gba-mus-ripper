# Windows
#CPPC=x86_64-w64-mingw32-g++.exe -std=gnu++11
#CC=x86_64-w64-mingw32-gcc.exe -std=c99

# GNU/Linux
CPPC=/usr/bin/g++ -std=gnu++11
CC=/usr/bin/gcc -std=c99

# Parameters used for compilation
FLAGS=-Wall -fdata-sections -ffunction-sections -fmax-errors=5 -Os
# Additional parameters used for linking whole programs
WHOLE=-s -fwhole-program -static

all: $(shell mkdir build) $(shell mkdir bin) bin/sappy_detector bin/song_ripper bin/sound_font_ripper bin/gba_mus_ripper

bin/sappy_detector: src/sappy_detector.c
	$(CC) $(FLAGS) $(WHOLE) src/sappy_detector.c -o bin/sappy_detector

bin/song_ripper: src/song_ripper.cpp src/midi.hpp build/midi.o
	$(CPPC) $(FLAGS) $(WHOLE) src/song_ripper.cpp build/midi.o -o bin/song_ripper

bin/sound_font_ripper: build/sound_font_ripper.o build/gba_samples.o build/gba_instr.o build/sf2.o
	$(CPPC) $(FLAGS) $(WHOLE) build/gba_samples.o build/gba_instr.o build/sf2.o build/sound_font_ripper.o -o bin/sound_font_ripper

bin/gba_mus_ripper: src/gba_mus_ripper.cpp src/hex_string.hpp
	$(CPPC) $(FLAGS) $(WHOLE) src/gba_mus_ripper.cpp -o bin/gba_mus_ripper

build/midi.o: src/midi.cpp src/midi.hpp
	$(CPPC) $(FLAGS) -c src/midi.cpp -o build/midi.o

build/gba_samples.o : src/gba_samples.cpp src/gba_samples.hpp src/hex_string.hpp src/sf2.hpp src/sf2_types.hpp
	$(CPPC) $(FLAGS) -c src/gba_samples.cpp -o build/gba_samples.o

build/gba_instr.o : src/gba_instr.cpp src/gba_instr.hpp src/sf2.hpp src/sf2_types.hpp src/hex_string.hpp src/gba_samples.hpp
	$(CPPC) $(FLAGS) -c src/gba_instr.cpp -o build/gba_instr.o

build/sf2.o : src/sf2.cpp src/sf2.hpp src/sf2_types.hpp src/sf2_chunks.hpp
	$(CPPC) $(FLAGS) -c src/sf2.cpp -o build/sf2.o

build/sound_font_ripper.o: src/sound_font_ripper.cpp src/sf2.hpp src/gba_instr.hpp src/hex_string.hpp
	$(CPPC) $(FLAGS) -c src/sound_font_ripper.cpp -o build/sound_font_ripper.o

clean:
	rm -f *.o *.s *.i *.ii
	rm -rf build/
	rm -rf bin/
