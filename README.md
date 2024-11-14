# GBA Mus Ripper 

GBA Mus Ripper by Bregalad and CaptainSwag101 (updated by berg8793, IAG and Thysbelon)

README originally written by Bregalad; edited by Thysbelon.

Downloads are available in the [releases section](https://github.com/Thysbelon/gba-mus-ripper/releases) on the right of this repository's GitHub homepage.

---

GBA Mus Ripper is a suite of programs to rip music from Game Boy Advance (GBA) games using the MusicPlayer2000 a.k.a. "Sappy" sound engine, a [really common engine among commercial GBA games](https://vgmpf.com/Wiki/index.php?title=User:IgoreshaZhu/GBA_sound_drivers#MusicPlayer2000.2FSappy_.28Nintendo.29).

---

## Applications:

- Listen to GBA music with a higher bit depth than 8 and no white noise.
- Apply the instrument set from a GBA game to another midi file.
- Simply get sheet music of a GBA game's tracks.
- Edit a song for a romhack by editing its Midi conversion, then convert it back to MP2K and inject it into the rom with [Midi2AGB](https://github.com/Thysbelon/midi2agb) and [gba-song-assembler](https://github.com/Thysbelon/gba-song-assembler). (The options `-xg -sb -rc -raw` should be used for this purpose)

The tools come in the form of 4 separate executable files.

IMPORTANT NOTE: The "gba_mus_ripper" executable simply calls the other 3 executables in the process of ripping the music, so even if you're among the 99% of users that will only use "gba_mus_ripper" and never touch the other 3 executables, do NOT remove, move or rename them, because "gba_mus_ripper" would also stop working!

---

### 1) GBA Mus Ripper

This is the main program which does everything automatically. Normally you'd only want to use this one, but the other 3 sub-programs can also be used individually too.
This program detects the presence of the MP2K sound engine in a given ROM, and converts all detected songs to .mid files, and rip all detected sound banks to a .sf2 file.

# HOW TO USE THIS PROGRAM

On Windows, you should open a console prompt in the folder containing the ".exe" files.
Type "cmd" in the File Explorer address bar, then press "enter". [(How-To Geek's guide)](https://www.howtogeek.com/789662/how-to-open-a-cmd-window-in-a-folder-on-windows/)

Now that the console prompt is open in your folder, type the following (parenthesis is a placeholder for a required input, brackets are a placeholder for optional settings):   
`gba_mus_ripper (input_file) [-o output_directory] [address] [flags]`

`input_file`: The FULL path name (in quotes) of the GBA ROM you want to rip from. (Hint: You can drag and drop the ROM into the command line)

`output_directory`: Again, a FULL path name (in quotes) of where you want MIDIs and soundfonts to be saved. If this parameter is not specified, the output directory will automatically be the same folder as where your ROM is located.

`address`: This is an offset address in hexadecimal format for when mp2ktool can't automatically detect the sound table address and you have to rip a ROM semi-manually<!-- (see said section below for more details)-->.

The following flag options are available:

`-gm` : Give General MIDI names to presets. Note that this will only change the names and will NOT
      magically turn the soundfont into a General MIDI compliant soundfont.
      
`-xg` : Output MIDI will be compliant to XG standard (instead of default GS standard)

`-rc` : Rearrange channels in output MIDIs so channel 10 is avoided. Needed by sound
      cards where it's impossible to disable "drums" on channel 10 even with GS or XG commands
      
`-sb` : Separate banks. Every sound bank is ripped to a different .sf2 file and placed
      into different sub-folders (instead of doing it in a single .sf2 file and a single folder). 
      (Some games' sound banks won't rip correctly unless this option is enabled. e.g. Castlevania: Circle of the Moon)
      
`-raw` : Output MIDIs exactly as they're encoded in ROM, without linearising volume and
       velocities<!-- and without simulating vibratos-->.

<!--On Linux, you should first compile the program yourself. See compilation notes a few chapters below.
Once the program is compiled into executable form, its usage is identical.-->

### 2) mp2ktool

This is a program [written by loveemu](https://github.com/loveemu/loveemu-lab/tree/master/gba/mp2ktool). Its purpose here is to detect the MP2K sound engine. There are various operations the user can use to print various kinds of info about how a game uses the MP2K engine; a list of these operations can be viewed by running just `mp2ktool`.

Using mp2ktool is the easiest way to know if a given GBA game uses the MP2K sound engine or not.

Usage: `mp2ktool <operation>`

Operations:

```
info ROM.gba
  search m4a block and show their basic info.
songlist (SongTable address) ROM.gba
  show list of items in song table.

songtable ROM.gba
  search song table offset.
songtableptr ROM.gba
  search song table pointer offset.
mplaytable ROM.gba
  search music player table offset.
ofslist ROM.gba
  show table offsets in format of gba2midi ofslist.txt
ofslist+ ROM.gba
  show table offsets in format of m4aroms.csv

header romtitle ROM.gba
  show 4 bytes ROM title in GBA ROM header.
header romid ROM.gba
  show 4 bytes ROM ID in GBA ROM header.
```

### 3) Song Ripper

This program rips sequence data from a GBA game using the MP2K sound engine to MIDI (.mid) format. You'd typically use this if you'd like to get the spreadsheet of a particular song with more options available than a plain dump with the default parameters from GBA Mus Ripper.

Usage: `song_ripper infile.gba outfile.mid song_address [flags]`

`-b[n]` : Bank: forces all patches to be in the specified bank (0-127)

`-gm` : Give General MIDI names to presets. Note that this will only change the names and will NOT magically turn the soundfont into a General MIDI compliant soundfont.

In general MIDI, midi channel 10 is reserved for drums. Unfortunately, we do not want to use any "drums" in the output file. There are 3 modes to fix this problem:

`-rc` : Rearrange Channels. This will avoid using the channel 10, and use it at last resort only if all 16 channels should be used

`-gs` : This will send a GS system exclusive message to tell the player channel 10 is not "drums"

`-xg` : This will send a XG system exclusive message, and force banks number which will disable "drums"

`-lv` : Linearise volume and velocities. This should be used to have the output "sound" like the original song, but shouldn't be used to get an exact dump of sequence data.

<!--`-sv` : Simulate vibrato. This will insert controllers in real time to simulate a vibrato, instead of just when commands are given. Like -lv, this should be used to have the output "sound" like the original song,but shouldn't be used to get an exact dump of sequence data.-->

#### Explanation of Special MP2K Midi CCs

Not all the features of MP2K are supported in the default midi spec, so song_ripper assigns these MP2K events to undefined Midi CCs, then modulators in the SF2 file allow these CCs to have the same effect as on MP2K.

For example, there is no Midi CC that is labelled as a way to control the speed of LFO (vibrato or tremolo), but MP2K has an LFOS event that is used to control the speed of LFO. To make the midi and SF2 more accurate to hardware, LFOS events in MP2K are converted to undefined CC21 events, and a modulator in the SF2 file uses CC21 as an input to control the LFO speed.

A list of all special undefined midi CCs, and what they are set to control, is listed below.

**NOTICE: a lot of SF2 synthesizer daw plugins (vst, lv2)** ***do not*** **support modulators.** Please use one of the options listed in the section ["Playback converted MIDIs"](#playback-converted-midis).

- CC21: LFO speed. The higher the number, the faster that LFO (vibrato or tremolo) will be. Takes the place of the MP2K event LFOS. If you'd like to see exactly how LFOS or CC21 relates to LFO speed, please refer to this [simple LFOS-CC21-Hertz Comparison webapp I made](https://thysbelon.github.io/gba-mus-ripper/mp2k-LFO-speed-and-sf2-LFO-speed-comparison.html).
- CC115: LFO speed in MP2K is determined by both the LFOS setting *and* the current BPM. There is no way for an SF2 modulator to read the current BPM of the Midi file, so instead a CC115 is placed on every track every time there is a BPM change in order to keep the LFO speed accurate to MP2K. The value of CC115 is the BPM divided by 4 (Midi CCs can only store a limited range of numbers).
- CC26: LFO delay. When this is greater than zero, every time a note plays, there will be a delay before LFO starts. The higher the number, the greater the delay.
- CC110: The first of two Midi CCs that take the place of MP2K MODT. MP2K MODT is a single event that determines if the LFO will be pitch (vibrato), volume (tremolo), or pan position (autopan). Because of the limitations of SF2 modulators, we need a separate CC to enable and disable each of these LFO types. CC110 controls pitch (vibrato) LFO type. **When CC110 is 0, pitch LFO type is enabled; when CC110 is 127 (max), pitch LFO type is disabled**. On MP2K, when no MODT event is used, LFO type defaults to pitch; using 0 as the "on" value for CC110 ensures the same is true in Midi and SF2.
- CC111: The second of two Midi CCs that take the place of MP2K MODT. CC111 controls volume (tremolo) LFO type. **When CC111 is 127, volume LFO type is enabled; when CC111 is 0, volume LFO type is disabled**. The reason the on and off values for CC110 and CC111 are different is to ensure that if neither CC110 or CC111 is set, the Midi and SF2 will default to pitch LFO type just like MP2K.  
Although Midi makes it possible to enable 2 kinds of LFO at the same time, please do not do this because it is not accurate to MP2K and it will cause issues if you try to run the Midi through [Midi2AGB](https://github.com/Thysbelon/midi2agb).  
In order for a Midi to successfully be converted by [Midi2AGB](https://github.com/Thysbelon/midi2agb), CC110 and CC111 *must* be grouped together, and the order of them must be CC110 *then* CC111.  
There is no Midi CC to set LFO type to pan position because it is impossible to do this with SF2 modulators. Whenever there is pan position LFO in an MP2K song, song_ripper will use CC110 and CC111 to disable pitch and volume LFO.
- CC24: Detune. This doesn't have any effect on the sound of the Midi and SF2, as an RPN 1 event is used to do detune instead. CC24 is still important if you plan on converting your Midi into MP2K, as only CC24 will be detected by Midi2AGB. A value of 64 is normal pitch, 0 is one semitone lower, and 127 is one semitone higher.
- "rev=???": This is a text event that controls what the global reverb of the song will be after being converted by Midi2AGB. It has no effect on the sound of the Midi and SF2.
- "pri=???": This is a text event that controls what the global priority of the song will be after being converted by Midi2AGB. Priority is a setting used by a GBA game engine to determine whether or not sound effects should interrupt this song. It has no effect on the sound of the Midi and SF2.
- "nat=???": This is a text event that controls whether Midi2AGB will alter the values of all volume events to approximate Midi-like loudness in MP2K, or leave them as is. It has no effect on the sound of the Midi and SF2. gba-mus-ripper always tries to preserve the volume levels of the original MP2K song, either by changing the volume event values, or by adding a modulator to the SF2 when raw is used. It is recommend to *not* change the "nat" event.

#### Explanation of RPN Events

[RecordingBlogs' explanation of RPN](https://www.recordingblogs.com/wiki/midi-registered-parameter-number-rpn)

### 4) Sound Font Ripper

Dumps a sound bank (or a list of sound banks) from a GBA game which is using the MP2K sound engine to SoundFont 2 (.sf2) format. You'd typically use this to get a SoundFont dump of data within a GBA game directly without dumping any other data.

Usage: `sound_font_ripper in.gba out.sf2 [flags] address1 [address2] ....`

Instruments at address1 will be dumped to Bank0, instruments at address2 to Bank1, etc.....

Flags:

`-v` : verbose: Display info about the sound font in text format. If `-v` is followed directly by a file name (as in `-vmyfile.txt`), info is printed to the specified file instead.

`-s` : Sampling rate for samples. Default: 22050 Hz

`-gm` : Give General MIDI names to presets. Note that this will only change the names and will NOT magically turn the soundfont into a General MIDI compliant soundfont.

`-mv` : Main volume for sample instruments. Range: 1-15. Game Boy channels are unaffected.

IMPORTANT NOTE: You need to leave the included file `psg_data.raw` and `goldensun_synth.raw` INTACT for Sound Font Ripper to work properly. If you remove or affect the files in any way, the "old" Game Boy PSG instruments and Golden Sun's synth instrument (respectively) won't be dumped at all.

---

## HOWTO:

### Playback converted MIDIs

Both MIDI and SF2 are widely used standards so there are a wide range of programs supporting these.

I recommend using a synthesizer that uses the FluidSynth library.   
FluidSynth is a pedantic midi and SF2 synthesizer library that focuses on following the midi and SF2 standard closely. It supports modulators in SF2 files, which this program uses to have pitch and volume LFO sound accurate to GBA. I always use FluidSynth when testing this program.

[VLC Media Player has a midi plugin that uses FluidSynth](https://wiki.videolan.org/Midi/). This plugin also supports choosing whatever soundfont file you want (either through the GUI, or you can run VLC from the command line with `vlc --soundfont input.sf2 input.midi` to temporarily change the soundfont).

For viewing and editing midi files, I recommend using [Reaper](https://www.reaper.fm/) (import midi with a separate track for each channel), [FluidSynthPlugin](https://github.com/prof-spock/FluidSynthPlugin), and [SWS actions to quickly copy and paste FluidSynthPlugin to all tracks](https://forums.cockos.com/showpost.php?p=1793337&postcount=4).

Another option for editing midi files is using MidiEditor connected to QSynth (GUI for the FluidSynth program).

I still need to research how to connect QSynth to Reaper.

### Compile the GBA Mus Ripper suite

Compilation is the act of turning human-readable source code into machine code that a computer can run. Ready-to-run builds are in the releases section. Compilation is *only* neccessary if you are a developer who wishes to make modifications to this program, or there is no release build for your platform.

First, use git to clone this repository **with submodules**. The sf2cute and yaml-cpp libraries that the suite depends on are submodules, and if they are not present the gba_mus_ripper suite will not compile.

Use `git clone --recurse-submodules https://github.com/Thysbelon/gba-mus-ripper.git`

Or, if you already cloned the repository, run `git submodule update --init --recursive` to fetch submodules.

<!--First, you should edit the Makefile (don't worry it's a very simple one) to suit your needs (compiler, flags, etc...).-->
You need support for C++11, this means if you're using gcc you're going to need a version more recent than 4.8. It's probably compilable on 4.7.x but it's simpler to just update the compiler.

There are separate Makefiles for different platforms. The default Makefile is for Linux. Makefile-windows is for compiling on Windows. Makefile-windows-[x86/aarch64]-on-ubuntu is for cross-compiling to Windows on Linux. Both Windows native and cross compilation use the compiler [llvm-mingw](https://github.com/mstorsjo/llvm-mingw), because it is a portable download that requires little set-up.

Remember to add llvm-mingw to the "path" environment variable before compiling.

There are two toolchain files, mingw-x86.cmake and mingw-aarch64.cmake, that both contain a relative path to the llvm-mingw folder. Please edit these paths to point to the location of llvm-mingw on your machine.

**Makefile-windows is currently nonfunctional.** This is because the current default Linux Makefile calls CMake to compile the sf2cute and yaml-cpp libraries that the suite depends on. llvm-mingw does not come with a CMake equivalent and I do not know of any Windows CMake programs that are as easy to install as llvm-mingw.

<!--### Rip songs semi-manually

When the automatic detection fails, it's possible (and actually fairly easy) to rip the data in a semi-automatic way. Semi-automatic because you have to locate the song table yourself within the ROM, but then the songs and their sound fonts are still dumped automatically by GBA Mus Ripper (you don't have to call Song Ripper and Sound Font Ripper manually, although you could of course do this).

A list of song table locations for GBA ROMs has been posted online already, so check that out first before going any further: http://gsf.caitsith2.net/ofslist.txt

To locate the song table within the ROM is pretty simple, but you need an HEX editor with search. Search for the sequence:

```
0x01, 0x3c, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00
```

Congratulations, you just found an "unused instrument". If there is several of them next to each other then there is not a single doubt the GBA ROM uses sappy (despite it's header not being detected).

Once you found this parameter your hex editor so that it shows a multiple of 8 bytes per row (8, 16 or 24), and scroll down until you see "columns" of `0x00` and `0x08` appearing. It should look like this:

```
0x** 0x** 0x** 0x08 0x0* 0x00 0x0* 0x00 0x** 0x** 0x** 0x08 0x0* 0x00 0x0* 0x00

0x** 0x** 0x** 0x08 0x0* 0x00 0x0* 0x00 0x** 0x** 0x** 0x08 0x0* 0x00 0x0* 0x00

0x** 0x** 0x** 0x08 0x0* 0x00 0x0* 0x00 0x** 0x** 0x** 0x08 0x0* 0x00 0x0* 0x00
```

Now just find the location of the very first word of 4 bytes that ends in `0x08` (it should be 32-bit aligned), and congratulations, you found the pointer to the first song of the game. For example, if the location was `0xabcde0`, you should use: `gba_mus_ripper game.gba 0xabcde0`

(Also, that "x" in the second position of the address is MANDATORY for it to work properly)

It's not that complicated, is it?

Now, if this is still too complicated, there is yet another alternative. Search for a tool called "mp2ktool", which can locate this automatically (and more accurately than my own Sappy Detector). So just use:

`mp2ktool songtable game.gba`

and it will print the offset on the screen, then use this offset as an argument to gba_mus_ripper and it'll rip it.
I only heard about this tool very recently but it sounds interesting. I'd have to give it a deeper investigation.
-->
---

<!--### Note about the old Java version 1.1

The version 1.1 of the program, written in Java and requiring the JRE to run, is still provided as archive for fallback for people who can't run nor compile the new version. This version is not supported by any means. It's usage and functionality is roughly equivalent to v2.0 with less features.

--- -->

## FREQUENTLY ASKED QUESTIONS

Q: How do I know if game XXX uses the MP2K engine?

A: Just try to run mp2ktool on it, if it doesn't work then it's very likely the game doesn't use the MP2K engine. A good 80-90% of commercial GBA games uses this engine. There are also multiple lists of every game that uses the MP2K sound engine: [IgoreshaZhu's list of GBA games by sound engine](https://vgmpf.com/Wiki/index.php?title=User:IgoreshaZhu/GBA_sound_drivers), [loveemu's Game Boy Advance Sound Driver List](https://gist.github.com/loveemu/5475a80e9fba4e71affbf9ca7a939ae5), [ofslist.txt: List of games using MusicPlayer2000](https://www.caitsith2.com/gsf/ofslist.txt)

<!--Q: If the sappy detector doesn't detect the engine header, does this mean the game doesn't use the sappy engine?

A: No. The detector is heuristic. There is games that uses sappy and that have no header. Metroid games comes in mind. Since v2.0 it's still possible to rip them by manually telling gba_mus_ripper the song table address. It will just not know the base sample rate and main volume, and use default values for them. This could affect the audibility of the music significantly.
On the other side, there might be fake positives, that is cases where the detector "thinks" there is a sappy header when there is none. If that's the case the program will start to jump garbage, but will probably encounter an error before the thing terminates. If this happens, you'd also have to give the address of the song table manually. TODO: shorten this to just the bit about metroid being difficult to detect volume. Figure out if there's a variant of MP2K that mp2ktool can't detect -->

Q: Will it sound EXACTLY like on the GBA?

A: No. It will sound close, but it is impossible to recreate GBA sound exactly with Midi and SF2. Though the Midi and SF2 may sound "better" in a sense due to not having the GBA's white noise, there are many subtle effects in GBA music that will be dropped in the conversion. For example: reverb doesn't sound the same, pan position LFO (an instrument bouncing between the left and right ears) is unsupported in Midi and SF2, etc. To get an idea of how different it is, try listening to this [comparison between the opening flute of Sky Tower on Midi and SF2 vs on mGBA](https://thysbelon.github.io/gba-mus-ripper/sky-tower-solo-flute-midi-mgba-comparison.flac) (The first half is Midi and SF2, the second half is mGBA). (Note: mGBA is also not completely accurate to a real GBA, but it is *much* closer than Midi and SF2)

The GameBoy instruments will sound different because the're emulated using samples. These instruments are quantified in volume and pitch, and there is no way to reproduce this imperfection in the .SF2 standard. Bregalad recorded the samples from a DS in GBA mode.

The Golden Sun synth instrument, added in version 2.2, also does not sound exactly like its original counterpart, again because it's emulated using samples. The assembly code for those instruments was unrolled and extremely optimised, therefore it was very hard to know exactly how those instruments were designed.

Q: Can I re-use the soundfont to play other MIDIs / Can I use soundfont from Game A to play music from Game B?

A: Yes but unfortunately there is a 99% of chances it will sound like total crap, because very few games have a General MIDI compliant soundfont. If you're looking for games with partially Genral Midi compliant soundfonts, there's Golden Sun and Castlevania: Aria of Sorrow. (All of these have a lot of missing instruments and some non-General MIDI compilant too so in all cases you'd need some MIDI editing to get the sound you'd expect). There are also some romhacks for games that make the sound banks General MIDI compliant.

If you accept however to do some MIDI editing and/or some SoundFont editing manually, then the answer to both of these questions is of course yes, and GBA Mus Ripper will probably be a great help.

Q: Can I also rip voices/sound effects?

A: Yes absolutely. In fact the engine does not differenciate music, sound effects and voices in any way so you'll rip them all automatically.

Q: The first song/first few songs works fine, but the following songs doesn't work.

A: This is probably a problem with using various sound font banks. You should be sure the driver and MIDI player you use supports banks. If it doesn't work in normal (GS) mode try to use the -xg argument for XG mode maybe you'll get better results.

Q: The song is mostly fine, but one track sounds weird/is missing

A: Maybe your driver doesn't support the GS message to disable "drums" on midi channel 10. The way sound_font_ripper converts the sound bank to soundfont requires that no channel is ever using MIDI "drums" (even if there are drums in the song - they'll be mapped to a normal instrument). You should try the `-rc` (rearrange channels) and/or `-xg` options.

<!--Q: Some Game Boy square wave part is missing in some songs in some games.

A: Again this is (sadly) normal. It means there is a supposedly "unused" instrument which in fact is used. The easiest fix is to fix the MIDIs in order to locate the part which don't play and change it's instrument to another equivalent one. Another approach would be to create the preset manually in the Sound Font file by copying another existing equivalent instrument. NOTE: I believe this has been fixed -->

Q: Why does it takes so long to rip the songs/soundfont?

A: It really depends on the game. Some games use a different sound bank for every single song, so those games takes a very long time to rip. In all cases it's normal the conversion can't be done instantly.

Q: Can you adapt this ripper to work with Game X which doesn't use the MP2K sound engine?

Q: Can you make this ripper work with GameBoy Color / Nintendo DS / any other console games?

A: No, because other sound engines work completely differently.

Q: The soundfonts cause major trouble / crash the program when I'm trying to use them

A: Some games uses too many different soundbanks for different songs and this causes problems in extreme cases (more than 128 banks). You should use the -sb command to separate sound banks into different folders, which will result in many soundfont files instead of a single soundfont with many sound banks.
