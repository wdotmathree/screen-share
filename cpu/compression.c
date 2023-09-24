#include "compression.h"

uint8_t *old_out = NULL;
uint8_t *old_in = NULL;

static const float quantization_table[8][8] = {
	{16, 11, 10, 16, 24, 40, 51, 61},	  {12, 12, 14, 19, 26, 58, 60, 55},
	{14, 13, 16, 24, 40, 57, 69, 56},	  {14, 17, 22, 29, 51, 87, 80, 62},
	{18, 22, 37, 56, 68, 109, 103, 77},	  {24, 35, 55, 64, 81, 104, 113, 92},
	{49, 64, 78, 87, 103, 121, 120, 101}, {72, 92, 95, 98, 112, 100, 103, 99},
};

#define ISQRT2 0.7071067811865475244008443621048490392848359376887f

static const float coeff[8][8] = {
	{0.5, ISQRT2, ISQRT2, ISQRT2, ISQRT2, ISQRT2, ISQRT2, ISQRT2},
	{ISQRT2, 1, 1, 1, 1, 1, 1, 1},
	{ISQRT2, 1, 1, 1, 1, 1, 1, 1},
	{ISQRT2, 1, 1, 1, 1, 1, 1, 1},
	{ISQRT2, 1, 1, 1, 1, 1, 1, 1},
	{ISQRT2, 1, 1, 1, 1, 1, 1, 1},
	{ISQRT2, 1, 1, 1, 1, 1, 1, 1},
	{ISQRT2, 1, 1, 1, 1, 1, 1, 1},
};

size_t compress_img(uint8_t *out, uint8_t *in, const int w, const int h) {
	// Compress the image in 8x8 blocks.
	size_t outlen = 0;
	for (int i = 0; i < w; i += 8) {
		for (int j = 0; j < h; j += 8) {
			int8_t tmp[8][8][3];
			for (int k = 0; k < 8; k++) {
				for (int l = 0; l < 8; l++) {
					uint8_t b = in[((i + k) + (j + l) * w) * 4 + 0];
					uint8_t g = in[((i + k) + (j + l) * w) * 4 + 1];
					uint8_t r = in[((i + k) + (j + l) * w) * 4 + 2];
					// tmp[k][l][0] = (0.299 * r + 0.587 * g + 0.114 * b) - 128;
					// tmp[k][l][1] = -0.168736 * r - 0.331264 * g + 0.5 * b;
					// tmp[k][l][2] = 0.5 * r - 0.418688 * g - 0.081312 * b;
					tmp[k][l][0] = r - 128;
					tmp[k][l][1] = g - 128;
					tmp[k][l][2] = b - 128;
				}
			}
			// Perform DCT
			int8_t dct[8][8][3] = {0};
			for (int u = 0; u < 8; u++) {
				for (int v = 0; v < 8; v++) {
					float t[3] = {0};
					for (int x = 0; x < 8; x++) {
						for (int y = 0; y < 8; y++) {
							t[0] += tmp[x][y][0] * cos((2 * x + 1) * u * M_PI / 16) *
									cos((2 * y + 1) * v * M_PI / 16);
							t[1] += tmp[x][y][1] * cos((2 * x + 1) * u * M_PI / 16) *
									cos((2 * y + 1) * v * M_PI / 16);
							t[2] += tmp[x][y][2] * cos((2 * x + 1) * u * M_PI / 16) *
									cos((2 * y + 1) * v * M_PI / 16);
						}
					}
					t[0] *= coeff[u][v] * 0.25f;
					t[1] *= coeff[u][v] * 0.25f;
					t[2] *= coeff[u][v] * 0.25f;
					// Quantize
					dct[u][v][0] = round(t[0] / quantization_table[u][v]);
					dct[u][v][1] = round(t[1] / quantization_table[u][v]);
					dct[u][v][2] = round(t[2] / quantization_table[u][v]);
				}
			}
			// Write to output
			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 8; l++) {
					for (int m = 0; m < 8; m++) {
						uint8_t b = dct[l][m][k];
						out[outlen++] = b;
					}
				}
			}
		}
	}
	return outlen;
}

void decompress_img(uint8_t *out, const uint8_t *in, size_t len, const int w, const int h) {
	int ii = 0;
	for (int i = 0; i < w; i += 8) {
		for (int j = 0; j < h; j += 8) {
			int8_t dct[8][8][3] = {0};
			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 8; l++) {
					for (int m = 0; m < 8; m++) {
						dct[l][m][k] = in[ii++];
					}
				}
			}
			// Dequantize
			int16_t tmp[8][8][3] = {0};
			for (int k1 = 0; k1 < 8; k1++) {
				for (int k2 = 0; k2 < 8; k2++) {
					tmp[k1][k2][0] = dct[k1][k2][0] * quantization_table[k1][k2];
					tmp[k1][k2][1] = dct[k1][k2][1] * quantization_table[k1][k2];
					tmp[k1][k2][2] = dct[k1][k2][2] * quantization_table[k1][k2];
				}
			}
			// Perform IDCT
			int8_t idct[8][8][3] = {0};
			for (int x = 0; x < 8; x++) {
				for (int y = 0; y < 8; y++) {
					float t[3] = {0};
					for (int u = 0; u < 8; u++) {
						for (int v = 0; v < 8; v++) {
							t[0] += coeff[u][v] * tmp[u][v][0] * cos((2 * x + 1) * u * M_PI / 16) *
									cos((2 * y + 1) * v * M_PI / 16);
							t[1] += coeff[u][v] * tmp[u][v][1] * cos((2 * x + 1) * u * M_PI / 16) *
									cos((2 * y + 1) * v * M_PI / 16);
							t[2] += coeff[u][v] * tmp[u][v][2] * cos((2 * x + 1) * u * M_PI / 16) *
									cos((2 * y + 1) * v * M_PI / 16);
						}
					}
					t[0] *= 0.25f;
					t[1] *= 0.25f;
					t[2] *= 0.25f;
					idct[x][y][0] = fmaxf(fminf(round(t[0]), 127), -128);
					idct[x][y][1] = fmaxf(fminf(round(t[1]), 127), -128);
					idct[x][y][2] = fmaxf(fminf(round(t[2]), 127), -128);
				}
			}
			// Write to output
			for (int k = 0; k < 8; k++) {
				for (int l = 0; l < 8; l++) {
					int8_t y = idct[k][l][0];
					int8_t cb = idct[k][l][1];
					int8_t cr = idct[k][l][2];
					// uint8_t r = y + 1.402 * cr + 128;
					// uint8_t g = y - 0.344136 * cb - 0.714136 * cr + 128;
					// uint8_t b = y + 1.772 * cb + 128;
					uint8_t r = y + 128;
					uint8_t g = cb + 128;
					uint8_t b = cr + 128;
					out[((i + k) + (j + l) * w) * 4 + 0] = b;
					out[((i + k) + (j + l) * w) * 4 + 1] = g;
					out[((i + k) + (j + l) * w) * 4 + 2] = r;
					out[((i + k) + (j + l) * w) * 4 + 3] = 255;
				}
			}
		}
	}
}
