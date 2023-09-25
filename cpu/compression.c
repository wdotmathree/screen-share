#include "compression.h"

uint8_t *old_out = NULL;
uint8_t *old_in = NULL;

// Quantization tables for F=50
static const float Qlum[8][8] = {
	{16, 11, 10, 16, 24, 40, 51, 61},	  {12, 12, 14, 19, 26, 58, 60, 55},
	{14, 13, 16, 24, 40, 57, 69, 56},	  {14, 17, 22, 29, 51, 87, 80, 62},
	{18, 22, 37, 56, 68, 109, 103, 77},	  {24, 35, 55, 64, 81, 104, 113, 92},
	{49, 64, 78, 87, 103, 121, 120, 101}, {72, 92, 95, 98, 112, 100, 103, 99},
};

static const float Qchrom[8][8] = {
	{17, 18, 24, 47, 99, 99, 99, 99}, {18, 21, 26, 66, 99, 99, 99, 99},
	{24, 26, 56, 99, 99, 99, 99, 99}, {47, 66, 99, 99, 99, 99, 99, 99},
	{99, 99, 99, 99, 99, 99, 99, 99}, {99, 99, 99, 99, 99, 99, 99, 99},
	{99, 99, 99, 99, 99, 99, 99, 99}, {99, 99, 99, 99, 99, 99, 99, 99},
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
	int8_t prevdc[3] = {0};
	size_t outlen = 0;
	for (int i = 0; i < w; i += 8) {
		for (int j = 0; j < h; j += 8) {
			// Convert into YCbCr
			int8_t tmp[8][8][3];
			for (int k = 0; k < 8; k++) {
				for (int l = 0; l < 8; l++) {
					uint8_t b = in[((i + k) + (j + l) * w) * 4 + 0];
					uint8_t g = in[((i + k) + (j + l) * w) * 4 + 1];
					uint8_t r = in[((i + k) + (j + l) * w) * 4 + 2];
					tmp[k][l][0] =
						fmaxf(fminf((0.299f * r + 0.587f * g + 0.114f * b) - 128, 127), -128);
					tmp[k][l][1] =
						fmaxf(fminf(-0.168736f * r - 0.331264f * g + 0.5f * b, 127), -128);
					tmp[k][l][2] =
						fmaxf(fminf(0.5f * r - 0.418688f * g - 0.081312f * b, 127), -128);
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
					dct[u][v][0] = round(t[0] / Qlum[u][v]);
					dct[u][v][1] = round(t[1] / Qchrom[u][v]);
					dct[u][v][2] = round(t[2] / Qchrom[u][v]);
				}
			}
			dct[0][0][0] -= prevdc[0];
			dct[0][0][1] -= prevdc[1];
			dct[0][0][2] -= prevdc[2];
			prevdc[0] += dct[0][0][0];
			prevdc[1] += dct[0][0][1];
			prevdc[2] += dct[0][0][2];
			// Zigzag
			int8_t zigzag[64][3] = {0};
			int k = 0;
			int l = 0;
			int m = 0;
			bool dir = true;
			while (l != 7 || m != 7) {
				zigzag[k][0] = dct[l][m][0];
				zigzag[k][1] = dct[l][m][1];
				zigzag[k][2] = dct[l][m][2];
				if (dir) {
					if (m == 7) {
						l++;
						dir = false;
					} else if (l == 0) {
						m++;
						dir = false;
					} else {
						l--;
						m++;
					}
				} else {
					if (l == 7) {
						m++;
						dir = true;
					} else if (m == 0) {
						l++;
						dir = true;
					} else {
						l++;
						m--;
					}
				}
				k++;
			}
			zigzag[k][0] = dct[l][m][0];
			// Run-length encoding
			for (int l = 0; l < 3; l++) {
				uint8_t run = 0;
				out[outlen++] = zigzag[0][l];
				for (int k = 1; k < 64; k++) {
					if (zigzag[k][l] == 0) {
						run++;
					} else {
						out[outlen++] = run;
						out[outlen++] = zigzag[k][l];
						run = 0;
					}
				}
				if (run > 0) {
					out[outlen++] = 0x00;
					out[outlen++] = 0x00;
				}
			}
		}
	}
	return outlen;
}

void decompress_img(uint8_t *out, const uint8_t *in, size_t len, const int w, const int h) {
	int8_t prevdc[3] = {0};
	int ii = 0;
	// Decompress the image in 8x8 blocks.
	for (int i = 0; i < w; i += 8) {
		for (int j = 0; j < h; j += 8) {
			// Reverse entropy coding
			int8_t zigzag[3][64] = {0};
			for (int k = 0; k < 3; k++) {
				int l = 1;
				zigzag[0][k] = prevdc[k] + in[ii++];
				prevdc[k] = zigzag[0][k];
				while (l < 64) {
					uint8_t run = in[ii++];
					int8_t val = in[ii++];
					if (run == 0 && val == 0)
						break;
					l += run;
					zigzag[k][l] = val;
					l++;
				}
			}
			// Reverse zigzag
			int8_t dct[8][8][3] = {0};
			int k = 0;
			int l = 0;
			int m = 0;
			bool dir = true;
			while (l != 7 || m != 7) {
				dct[l][m][0] = zigzag[0][k];
				dct[l][m][1] = zigzag[1][k];
				dct[l][m][2] = zigzag[2][k];
				if (dir) {
					if (m == 7) {
						l++;
						dir = false;
					} else if (l == 0) {
						m++;
						dir = false;
					} else {
						l--;
						m++;
					}
				} else {
					if (l == 7) {
						m++;
						dir = true;
					} else if (m == 0) {
						l++;
						dir = true;
					} else {
						l++;
						m--;
					}
				}
				k++;
			}
			// Dequantize
			int16_t tmp[8][8][3] = {0};
			for (int k1 = 0; k1 < 8; k1++) {
				for (int k2 = 0; k2 < 8; k2++) {
					tmp[k1][k2][0] = dct[k1][k2][0] * Qlum[k1][k2];
					tmp[k1][k2][1] = dct[k1][k2][1] * Qchrom[k1][k2];
					tmp[k1][k2][2] = dct[k1][k2][2] * Qchrom[k1][k2];
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
					uint8_t r = fmaxf(fminf(y + 1.402f * cr, 127), -128) + 128;
					uint8_t g = fmaxf(fminf(y - 0.344136f * cb - 0.714136f * cr, 127), -128) + 128;
					uint8_t b = fmaxf(fminf(y + 1.772f * cb, 127), -128) + 128;
					out[((i + k) + (j + l) * w) * 4 + 0] = b;
					out[((i + k) + (j + l) * w) * 4 + 1] = g;
					out[((i + k) + (j + l) * w) * 4 + 2] = r;
					out[((i + k) + (j + l) * w) * 4 + 3] = 255;
				}
			}
		}
	}
}
