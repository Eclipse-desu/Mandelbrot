#ifndef __ARIMURA_ROMI__BMP_H__
#define __ARIMURA_ROMI__BMP_H__

#include <cstdint>
#include <cstdlib>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

struct BMPHeader {
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;


	BMPHeader() {
		bfReserved1 = 0;
		bfReserved2 = 0;
	}

	BMPHeader(WORD _bfType, DWORD _bfSize, DWORD _bfOffBits)
		: bfType(_bfType), bfSize(_bfSize), bfOffBits(_bfOffBits) {
			bfReserved1 = 0;
			bfReserved2 = 0;
		}
}__attribute__((packed));

struct BMPInfoHeader{
	DWORD biSize;
	DWORD biWidth;
	DWORD biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImages;
	DWORD biXPelsPerMeter;
	DWORD biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;

	BMPInfoHeader(DWORD _biWidth = 0, DWORD _biHeight = 0, DWORD _biBitCount = 24)
		: biWidth(_biWidth), biHeight(_biHeight), biBitCount(_biBitCount) {
			biSize = 40;
			biPlanes = 1;
			biCompression = 0;
			biSizeImages = 0;
			biXPelsPerMeter = 0x0ec4;
			biYPelsPerMeter = 0x0ec4;
			biClrUsed = 0;
			biClrImportant = 0;
		}
}__attribute__((packed));

BYTE WHITE[4] = {0xff, 0xff, 0xff, 0};
BYTE RED[4] = {0xff, 0xff, 0, 0};
BYTE GREEN[4] = {0, 0xff, 0, 0};
BYTE BLUE[4] = {0, 0, 0xff, 0};
BYTE BLACK[4] = {0, 0, 0, 0};

BYTE* color[] = {WHITE, RED, GREEN, BLUE, BLACK};

// the pic will be a 24-bit bmp
// the array element will be treated (_pic[pos] == 0) ? 0 : 1
int WriteBlackWhiteBMP(FILE* _out, unsigned char* _pic, int _row, int _col) {
	int BPP = 24;
	int rowsize = 4 * ((BPP * _row / 32) + (((BPP * _row) & 31) ? 1 : 0));
	// int rowsize = 4 * _row;
	printf("%d %d\n", _row, rowsize);
	DWORD size = 14 + 40 + rowsize * _col;
	printf("size: %x\n", size);
	BMPHeader Header(0x4d42, size, 14 + 40);
	BMPInfoHeader InfoHeader(_row, -_col, BPP);

	fwrite(&Header.bfType, sizeof(BYTE), sizeof(Header), _out);
	fwrite(&InfoHeader.biSize, sizeof(BYTE), sizeof(InfoHeader), _out);

	int align = (4 + BPP * _row / 32 * 4 - BPP / 8 * _row) % 4;
	printf("align: %d\n", align);
	unsigned char* cur = _pic;
	for (int c = 0; c < _col; c++) {
		for (int r = 0; r < _row; r++) {
			// printf("%d ", *cur);
			fwrite(color[*cur], sizeof(BYTE), BPP / 8, _out);
			cur++;
		}
		for (int a = 0; a < align; a++) {
			fwrite("\0", 1, 1, _out);
		}
	}

	return 0;
}

#endif