#pragma once
#ifndef _COMPRESSION_H
#define _COMPRESSION_H

#include "include.h"

/**
 * @brief Compress an image.
 * @param out Output
 * @param in Input image
 * @param w Width of the image
 * @param h Height of the image
 */
size_t compress_img(uint8_t *, uint8_t *, const int, const int);

/**
 * @brief Decompress an image.
 * @param out Output image
 * @param in Input
 * @param len Length of the input
 * @param w Width of the image
 * @param h Height of the image
 */
void decompress_img(uint8_t *, const uint8_t *, size_t, const int, const int);

#endif
