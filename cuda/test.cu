#include <iostream>

unsigned int *dev_in, *dev_out;

__global__ void downscale(unsigned int *in, unsigned int *out, int sw, int sh, int dw, int dh) {
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	int x2 = x * sw / dw;
	int y2 = y * sh / dh;
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

extern "C" int scale_img(unsigned int *in, unsigned int *out, int sw, int sh, int dw, int dh) {
	if (dev_in == NULL) {
		cudaMalloc((void **)&dev_in, sw * sh * 4);
		cudaMalloc((void **)&dev_out, dw * dh * 4);
	}
	cudaMemcpy(dev_in, in, sw * sh * 4, cudaMemcpyHostToDevice);

	dim3 threads(16, 16);
	dim3 blocks(dw / threads.x, dh / threads.y);
	downscale<<<blocks, threads>>>(dev_in, dev_out, sw, sh, dw, dh);
	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess) {
		std::cerr << cudaGetErrorString(err) << std::endl;
		return -1;
	}
	cudaMemcpy(out, dev_out, dw * dh * 4, cudaMemcpyDeviceToHost);
	return 0;
}
