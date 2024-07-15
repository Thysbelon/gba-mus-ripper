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

all: $(shell mkdir build) $(shell mkdir out) out/sappy_detector out/song_ripper out/sound_font_ripper out/gba_mus_ripper

bin/sappy_detector: sappy_detector.c
	$(CC) $(FLAGS) $(WHOLE) sappy_detector.c -o bin/sappy_detector

bin/song_ripper: song_ripper.cpp midi.hpp build/midi.o
	$(CPPC) $(FLAGS) $(WHOLE) song_ripper.cpp build/midi.o -o bin/song_ripper

bin/sound_font_ripper: build/sound_font_ripper.o build/gba_samples.o build/gba_instr.o build/sf2.o
	$(CPPC) $(FLAGS) $(WHOLE) build/gba_samples.o build/gba_instr.o build/sf2.o build/sound_font_ripper.o -o bin/sound_font_ripper

bin/gba_mus_ripper: gba_mus_ripper.cpp hex_string.hpp
	$(CPPC) $(FLAGS) $(WHOLE) gba_mus_ripper.cpp -o bin/gba_mus_ripper

build/midi.o: midi.cpp midi.hpp
	$(CPPC) $(FLAGS) -c midi.cpp -o build/midi.o

build/gba_samples.o : gba_samples.cpp gba_samples.hpp hex_string.hpp sf2.hpp sf2_types.hpp
	$(CPPC) $(FLAGS) -c gba_samples.cpp -o build/gba_samples.o

build/gba_instr.o : gba_instr.cpp gba_instr.hpp sf2.hpp sf2_types.hpp hex_string.hpp gba_samples.hpp
	$(CPPC) $(FLAGS) -c gba_instr.cpp -o build/gba_instr.o

build/sf2.o : sf2.cpp sf2.hpp sf2_types.hpp sf2_chunks.hpp
	$(CPPC) $(FLAGS) -c sf2.cpp -o build/sf2.o

build/sound_font_ripper.o: sound_font_ripper.cpp sf2.hpp gba_instr.hpp hex_string.hpp
	$(CPPC) $(FLAGS) -c sound_font_ripper.cpp -o build/sound_font_ripper.o

clean:
	rm -f *.o *.s *.i *.ii
	rm -rf build/
	rm -rf bin/
