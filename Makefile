# Windows
#CPPC=x86_64-w64-mingw32-g++.exe -std=gnu++11
#CC=x86_64-w64-mingw32-gcc.exe -std=c99

# GNU/Linux
CPPC=/usr/bin/g++ -std=gnu++11
CPPC2=/usr/bin/g++ -std=c++17
CC=/usr/bin/gcc -std=c99
CMAKE=/usr/bin/cmake
MAKE=/usr/bin/make

# Parameters used for compilation
FLAGS=-Wall -fdata-sections -ffunction-sections -fmax-errors=5 -Os
# Additional parameters used for linking whole programs
WHOLE=-s -fwhole-program -static

SF2CUTE_SRC_FILES = src/sf2cute/src/sf2cute/$(wildcard *.cpp) # TODO include *.hpp files too?

all: $(shell mkdir build) $(shell mkdir bin) bin/mp2ktool bin/song_ripper bin/sound_font_ripper bin/gba_mus_ripper

bin/mp2ktool: src/mp2ktool/mp2ktool.cpp src/mp2ktool/mp2kcomm.cpp src/mp2ktool/agbm4a.cpp src/mp2ktool/mp2kcomm.h src/mp2ktool/agbm4a.h
	$(CPPC2) src/mp2ktool/mp2ktool.cpp src/mp2ktool/mp2kcomm.cpp src/mp2ktool/agbm4a.cpp -o bin/mp2ktool

bin/song_ripper: src/song_ripper.cpp src/midi.hpp build/midi.o
	$(CPPC) $(FLAGS) $(WHOLE) src/song_ripper.cpp build/midi.o -o bin/song_ripper

bin/sound_font_ripper: build/sound_font_ripper.o build/gba_samples.o build/gba_instr.o src/sf2cute/build/libsf2cute.a
	$(CPPC) $(FLAGS) $(WHOLE) build/gba_samples.o build/gba_instr.o build/sound_font_ripper.o src/sf2cute/build/libsf2cute.a -o bin/sound_font_ripper

bin/gba_mus_ripper: src/gba_mus_ripper.cpp src/hex_string.hpp
	$(CPPC) $(FLAGS) $(WHOLE) src/gba_mus_ripper.cpp -o bin/gba_mus_ripper

build/midi.o: src/midi.cpp src/midi.hpp
	$(CPPC) $(FLAGS) -c src/midi.cpp -o build/midi.o

build/gba_samples.o : src/gba_samples.cpp src/gba_samples.hpp src/hex_string.hpp
	$(CPPC) $(FLAGS) -c src/gba_samples.cpp -o build/gba_samples.o

build/gba_instr.o : src/gba_instr.cpp src/gba_instr.hpp src/hex_string.hpp src/gba_samples.hpp
	$(CPPC) $(FLAGS) -c src/gba_instr.cpp -o build/gba_instr.o

build/sound_font_ripper.o: src/sound_font_ripper.cpp src/gba_instr.hpp src/hex_string.hpp
	$(CPPC) $(FLAGS) -c src/sound_font_ripper.cpp -o build/sound_font_ripper.o

src/sf2cute/build/libsf2cute.a: $(SF2CUTE_SRC_FILES)
	$(CMAKE) -S src/sf2cute -B src/sf2cute/build
	$(MAKE) --directory=src/sf2cute/build

clean:
	rm -f *.o *.s *.i *.ii
	rm -rf build/
	rm -rf bin/
