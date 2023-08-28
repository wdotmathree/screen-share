#include "cuda.h"

int scale_img(unsigned int *in, unsigned int *out, int sw, int sh, int dw, int dh) {
	for (int x = 0; x < dw; x++) {
		for (int y = 0; y < dh; y++) {
			int x2 = x * sw / dw;
			int y2 = y * sh / dh;
			out[x + y * dw] = in[x2 + y2 * sw];
			int x3 = (x + 1) * sw / dw;
			int y3 = (y + 1) * sh / dh;
			int r = 0, g = 0, b = 0;
			for (int i = x2; i < x3; i++) {
				for (int j = y2; j < y3; j++) {
					unsigned int pix = in[i + j * sw];
					r += pix & 0xff;
					g += (pix >> 8) & 0xff;
					b += (pix >> 16) & 0xff;
				}
			}
			r /= (x3 - x2) * (y3 - y2);
			g /= (x3 - x2) * (y3 - y2);
			b /= (x3 - x2) * (y3 - y2);
			out[x + y * dw] = r | (g << 8) | (b << 16);
		}
	}
	return 0;
}
