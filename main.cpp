#include <cstdio>
#include <complex>
#include <cstring>

#include "bmp.h"

#define pos(x, y) ((x + radii) * (2 * radii) + y + radii)

std::complex<double> convert(int x, int y, int unit) {
	return std::complex<double>((double)x / unit, (double)y / unit);
}

int main(int argc, char* argv[]) {
	// iternum is the maxinum iteration time
	int iternum = 100;
	// unit = pixels in one unit (0, 0)->(0, 1)
	int unit = 100;
	if (argc == 2) {
		sscanf(argv[1], "%d", &iternum);
	} else if(argc == 3) {
		sscanf(argv[1], "%d", &iternum);
		sscanf(argv[2], "%d", &unit);
	}
	// radii is for radius of the picture, the coord will be [-radii, radii].
	// ususally radii will be 2.1 unit
	int radii = 2.1 * unit;
	// pic is the picture plane
	unsigned char* pic = new unsigned char [(2 * radii + 1) * (2 * radii + 1)];
	memset(pic, 0, sizeof(pic));

	// generate the picture
	// cur is for "current position", because of the choice of iteraters, next position is just cur++.
	unsigned char* cur = pic;
	for (int i = -radii; i <= radii; i++) {
		for (int j = -radii; j <= radii; j++) {
			std::complex<double> c = convert(i, j, unit);

			bool flag = true;
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
	FILE *fout = fopen("out.bmp", "w");
	WriteBlackWhiteBMP(fout, pic, 2 * radii + 1, 2 * radii + 1);

	delete[] pic;
}