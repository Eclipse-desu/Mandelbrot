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

struct COLOR_RGBA {
	BYTE RGBA[4];

	COLOR_RGBA(int _r = 0, int _g = 0, int _b = 0, int _a = 0) {
		this->RGBA[2] = _r;
		this->RGBA[1] = _g;
		this->RGBA[0] = _b;
		this->RGBA[3] = _a;
	}

	BYTE& red() { return this->RGBA[2]; }
	BYTE& green() { return this->RGBA[1]; }
	BYTE& blue() { return this->RGBA[0]; }
	BYTE& alpha() { return this->RGBA[3]; }

	BYTE* ptr() { return RGBA; }

	bool readRGB(char* str) {
		int tmpv;
		char cache[3]; cache[2] = 0;
		for (int i = 2; i >= 0; i--) {
			cache[0] = str[2 * (2 - i)];
			cache[1] = str[2 * (2 - i) + 1];
			sscanf(cache, "%x", &tmpv);
			printf("%d\n", tmpv);
			RGBA[i] = tmpv;
		}
		RGBA[3] = 0;
		return true;
	}

	void print() {
		printf("%d %d %d %d | %02x%02x%02x-%02x\n", RGBA[2], RGBA[1], RGBA[0], RGBA[3], RGBA[2], RGBA[1], RGBA[0], RGBA[3]);
	}
};

COLOR_RGBA WHITE(0xff, 0xff, 0xff, 0);
COLOR_RGBA RED(0xff, 0, 0, 0);
COLOR_RGBA GREEN(0, 0xff, 0, 0);
COLOR_RGBA BLUE(0, 0, 0xff, 0);
COLOR_RGBA BLACK(0, 0, 0, 0);

COLOR_RGBA BlackWhite(int _n) {
	return _n == 0 ? WHITE : BLACK;
}

// function for 2-color function
// if data <= threshod, then it will be color 1. Default 0
int Write2ColorBMP(FILE* _out, unsigned char* _pic, int _row, int _col, int _threshod = 0, COLOR_RGBA color0 = BLACK, COLOR_RGBA color1 = WHITE) {
	const int BPP = 1;
	int rowsize = 4 * (BPP * _row / 32 + (((BPP * _row) & 31) ? 1 :0));
	printf("%d %d\n", _row, rowsize);
	DWORD size = 14 + 40 + 8 + rowsize * _col;
	BMPHeader Header(0x4d42, size, 14 + 40 + 8);
	BMPInfoHeader InfoHeader(_row, _col, BPP);

	fwrite(&Header.bfType, sizeof(BYTE), sizeof(Header), _out);
	fwrite(&InfoHeader.biSize, sizeof(BYTE), sizeof(InfoHeader), _out);
	fwrite(color0.ptr(), sizeof(BYTE), sizeof(color0), _out);
	fwrite(color1.ptr(), sizeof(BYTE), sizeof(color0), _out);

	int alignbits = rowsize * 8 - _row;
	printf("align: %d\n", alignbits);
	unsigned char* cur = _pic;
	for (int c = 0; c < _col; c++) {
		int curalign = alignbits;
		for (int r = 0; r < _row; r += 8) {
			BYTE arr = 0;
			// bool flag = false;
			for (int v = 0; v < 8; v++) {
				int curnum = 0;
				if (r + v < _row) {
					curnum = (*cur <= _threshod) ? 0 : 1; cur++;
				} else {
					curalign--;
				}
				arr = (arr << 1) + curnum;
				// if (flag) printf("%d ", arr);
			}
			fwrite(&arr, sizeof(BYTE), 1, _out);
		}
		if (curalign > 0) {
			fwrite(BLACK.ptr(), sizeof(BYTE), curalign / 8, _out);
		}
	}

	return 0;
}

// function for no color palette
int WriteBMP(FILE* _out, unsigned char* _pic, int _row, int _col, int BPP = 24, COLOR_RGBA (*color)(int) = BlackWhite) {
	int rowsize = 4 * ((BPP * _row / 32) + (((BPP * _row) & 31) ? 1 : 0));
	// int rowsize = 4 * _row;
	// printf("%d %d\n", _row, rowsize);
	DWORD size = 14 + 40 + rowsize * _col;
	printf("size: %x\n", size);
	BMPHeader Header(0x4d42, size, 14 + 40);
	BMPInfoHeader InfoHeader(_row, _col, BPP);

	fwrite(&Header.bfType, sizeof(BYTE), sizeof(Header), _out);
	fwrite(&InfoHeader.biSize, sizeof(BYTE), sizeof(InfoHeader), _out);

	int align = (4 + BPP * _row / 32 * 4 - BPP / 8 * _row) % 4;
	// printf("align: %d\n", align);
	unsigned char* cur = _pic;
	for (int c = 0; c < _col; c++) {
		for (int r = 0; r < _row; r++) {
			// printf("%d ", *cur);
			// printf("%d %d %d %d\n", color(*cur).red(), color(*cur).green(), color(*cur).blue(), color(*cur).alpha());
			fwrite(color(*cur).ptr(), sizeof(BYTE), BPP / 8, _out);
			cur++;
		}
		for (int a = 0; a < align; a++) {
			fwrite("\0", 1, 1, _out);
		}
	}

	return 0;
}

#endif