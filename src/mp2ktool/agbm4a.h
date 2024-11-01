// Small tool for AGB Music Player 2000 (aka. M4A, MP2000, Sappy)
// Programmed by loveemu, published under MIT License.
// Special Thanks To: Sappy, GSF Central, VGMTrans and its author.

#ifndef AGBM4A_INCLUDED
#define AGBM4A_INCLUDED

class M4ASong
{
private:
	M4ASong() { }

public:
	M4ASong(uint8_t *rom, uint32_t romsize, uint32_t songheader_offset);
	virtual ~M4ASong() { }

	void test();

private:
	uint8_t *ROM;
	uint32_t ROMSize;
	uint32_t SongHeaderOffset;
};

#endif // !AGBM4A_INCLUDED
