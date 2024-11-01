// Small tool for AGB Music Player 2000 (aka. M4A, MP2000, Sappy)
// Programmed by loveemu, published under MIT License.
// Special Thanks To: Sappy, GSF Central, VGMTrans and its author.

#include "mp2kcomm.h"
#include "agbm4a.h"

M4ASong::M4ASong(uint8_t *rom, uint32_t romsize, uint32_t songheader_offset)
{
	ROM = rom;
	ROMSize = romsize;
	SongHeaderOffset = songheader_offset;
}

void M4ASong::test()
{
	printf("%08X test\n", SongHeaderOffset);
}
