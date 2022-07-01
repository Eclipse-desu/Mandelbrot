#include <cmath>
#include <cstdio>
#include <complex>
#include <cstring>

#include "../include/bmp.h"

std::complex<double> convert(int x, int y, int unit) {
	return std::complex<double>((double)x / unit, (double)y / unit);
}

int main(int argc, char* argv[]) {
	// iternum is the maxinum iteration time
	int iternum = 100;
	// unit = pixels in one unit (0, 0)->(0, 1)
	int unit = 500;
	if (argc == 2) {
		sscanf(argv[1], "%d", &iternum);
	} else if(argc == 3) {
		sscanf(argv[1], "%d", &iternum);
		sscanf(argv[2], "%d", &unit);
	}
	// size of the picture
	// int width = 1920, height = 1080;
	int height = unit * 4.5, width = height;
	std::complex<double> origin = std::complex<double>(-width / (2.0 * unit), -height / (2.0 * unit));
	// pic is the picture plane
	unsigned char* pic = new unsigned char [width * height];
	memset(pic, 0, sizeof(pic));

	// generate the picture
	// cur is for "current position", because of the choice of iteraters, next position is just cur++.
	unsigned char* cur = pic;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			std::complex<double> c = convert(i, j, unit) + origin;

			int flag = true;
			std::complex<double> z = 0;
			for (int itertime = 0; itertime < iternum; itertime++) {
				if (std::norm(z) > 4) {
					flag = false;
					break;
				}
				z = z * z + c;
			}
			*cur = flag ? 1 : 0;
			cur++;
		}
	}

	// write file
	char *filename = new char [13 + 2 + 20];
	sprintf(filename, "../img/bw-%d-%d.bmp", iternum, unit);
	FILE *fout = fopen(filename, "w");
	Write2ColorBMP(fout, pic, width, height);
	delete[] filename;

	fclose(fout);
	delete[] pic;
}