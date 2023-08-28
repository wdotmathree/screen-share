#pragma once
#ifndef _CUDA_H
#define _CUDA_H

/**
 * @brief Scale an image.
 * @param in Input image
 * @param out Output image
 * @param sw Source width
 * @param sh Source height
 * @param dw Destination width
 * @param dh Destination height
 * @return 0 on success, or -1 on error.
 */
int scale_img(unsigned int *in, unsigned int *out, int sw, int sh, int dw, int dh);

#endif
