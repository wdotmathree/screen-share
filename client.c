#include "include.h"

#include "compression.h"
#include "cuda.h"
#include "ss.h"

int main(void) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_Window *window = SDL_CreateWindow(
		"Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN
	);
	SDL_Surface *s = SDL_GetWindowSurface(window);
	ss_init();
	int width = ss_get_width();
	int height = ss_get_height();
	uint8_t *buf = malloc(width * height * 4);
	uint8_t *uncompressed = malloc(1280 * 720 * 4);
	uint8_t *compressed = malloc(1280 * 720 * 4);
	SDL_Event e;
	uint64_t t = 0;
	uint32_t its = 0;
	bool stop = false;
	while (!stop) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				stop = true;
				break;
			} else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					stop = true;
					break;
				}
			}
		}
		ss_get_screenshot(buf);
		uint64_t tt = SDL_GetPerformanceCounter();
		scale_img((uint32_t *)buf, (uint32_t *)uncompressed, width, height, 1280, 720);
		size_t out_len = compress_img(compressed, uncompressed, 1280, 720);
		decompress_img((uint32_t *)s->pixels, compressed, out_len, 1280, 720);
		t += SDL_GetPerformanceCounter() - tt;
		SDL_UpdateWindowSurface(window);
		its++;
	}
	printf("%lf\n", (double)t / SDL_GetPerformanceFrequency() / its);
	free(buf);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
