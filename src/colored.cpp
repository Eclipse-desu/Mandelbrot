#include <cmath>
#include <cstdio>
#include <complex>
#include <cstring>

#include "../include/bmp.h"

std::complex<double> convert(int x, int y, int unit) {
	return std::complex<double>((double)x / unit, (double)y / unit);
}

COLOR_RGBA *colorset;

COLOR_RGBA gradient_mode(int num) {
	if (num == 255) return colorset[1];
	double control = std::log(1 - 0.25);
	double v = 1 - std::exp(control * num);
	COLOR_RGBA tmp = colorset[0];
	tmp.red() *= v; tmp.green() *= v; tmp.blue() *= v;
	// tmp.print();
	return tmp;
}

COLOR_RGBA bicolor_mode(int num) {
	if (num == 255) return colorset[2];
	return colorset[num & 1];
}

int main(int argc, char* argv[]) {
	// iternum is the maxinum iteration time
	int iternum = 100;
	// unit = pixels in one unit (0, 0)->(0, 1)
	int unit = 500;
	char mode;
	if (argc == 2) {
		sscanf(argv[1], "%d", &iternum);
	} else if (argc == 3) {
		sscanf(argv[1], "%d", &iternum);
		sscanf(argv[2], "%d", &unit);
	} else if (argc > 3) {
		sscanf(argv[1], "%d", &iternum);
		sscanf(argv[2], "%d", &unit);
		sscanf(argv[3], "-%c", &mode);
		if (mode == 'g') {
			colorset = new COLOR_RGBA[2];
			for (int i = 0; i < 2; i++) {
				colorset[i].readRGB(argv[4 + i]);
				// colorset[i].print();
			}
		} else if (mode == 'b') {
			colorset = new COLOR_RGBA[3];
			for (int i = 0; i < 3; i++) {
				colorset[i].readRGB(argv[4 + i]);
			}
		} else {
			printf("input error\n");
			return 0;
		}
	}

	// size of the picture
	// int width = 1920, height = 1080;
	int height = unit * 3, width = unit * 3;
	std::complex<double> origin = std::complex<double>(-2, -1.5);
	// pic is the picture plane
	unsigned char* pic = new unsigned char [width * height];
	memset(pic, 0, sizeof(pic));

	// generate the picture
	// cur is for "current position", because of the choice of iteraters, next position is just cur++.
	unsigned char* cur = pic;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			std::complex<double> c = convert(i, j, unit) + origin;

			int flag = 0;
			std::complex<double> z = 0;
			for (int itertime = 0; itertime < iternum; itertime++) {
				z = z * z + c;
				if (std::norm(z) > 4) {
					break;
				}
				flag++;
			}
			*cur = flag == iternum ? 255 : (flag < 255 ? flag : 254);
			cur++;
		}
	}

	// write file
	char *filename = new char [20 + 2 + 20];
	sprintf(filename, "../img/colored-%c-%d-%d.bmp", mode, iternum, unit);
	FILE *fout = fopen(filename, "w");
	WriteBMP(fout, pic, width, height, 24, mode == 'g' ? gradient_mode : bicolor_mode);
	delete[] filename;

	fclose(fout);
	delete[] pic;
	delete[] colorset;
}