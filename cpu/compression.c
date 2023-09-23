#include "compression.h"

uint8_t *old_out = NULL;
uint8_t *old_in = NULL;

size_t compress_img(uint8_t *out, uint8_t *in, const int w, const int h) {
	uint64_t t = SDL_GetPerformanceCounter();
	// Compress the image in 8x8 blocks.
	for (int i = 0; i < w; i += 8) {
		for (int j = 0; j < h; j += 8) {
			int8_t tmp[8][8][3];
			for (int k = 0; k < 8; k++) {
				for (int l = 0; l < 8; l++) {
					uint8_t r = in[((i + k) * w + (j + l)) * 4 + 0];
					uint8_t g = in[((i + k) * w + (j + l)) * 4 + 1];
					uint8_t b = in[((i + k) * w + (j + l)) * 4 + 2];
					tmp[k][l][0] = (0.299 * r + 0.587 * g + 0.114 * b);
					tmp[k][l][1] = 128 - 0.168736 * r - 0.331264 * g + 0.5 * b;
					tmp[k][l][2] = 128 + 0.5 * r - 0.418688 * g - 0.081312 * b;
				}
			}
			// Perform DCT
			uint16_t dct[3][8][8] = {0};
			for (int k1 = 0; k1 < 8; k1++) {
				for (int k2 = 0; k2 < 8; k2++) {
					for (int n1 = 0; n1 < 8; n1++) {
						for (int n2 = 0; n2 < 8; n2++) {
							dct[0][k1][k2] += tmp[n1][n2][0] * cos(M_PI / 8 * (n1 + 0.5) * k1) *
											  cos(M_PI / 8 * (n2 + 0.5) * k2);
							// dct[1][k1][k2] += tmp[n1][n2][1] * cos(M_PI / 8 * (n1 + 0.5) * k1) *
							// 				  cos(M_PI / 8 * (n2 + 0.5) * k2);
							// dct[1][k1][k2] += tmp[n1][n2][1] * cos(M_PI / 8 * (n1 + 0.5) * k1) *
							// 				  cos(M_PI / 8 * (n2 + 0.5) * k2);
						}
					}
				}
			}
			if (i == 320 && j == 320) {
				for (int k = 0; k < 8; k++) {
					for (int l = 0; l < 8; l++) {
						printf("%hd ", dct[0][k][l]);
					}
					printf("\n");
				}
			}
		}
	}
	printf("%f\n", (float)(SDL_GetPerformanceCounter() - t) / SDL_GetPerformanceFrequency());
	return 0;
}

void decompress_img(uint8_t *out, const uint8_t *in, size_t len, const int w, const int h) {}
