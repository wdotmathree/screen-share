#include "include.h"

#include "ss.h"

uint32_t avg(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
	uint32_t r = 0;
	r += (a >> 24) & 0xff;
	r += (b >> 24) & 0xff;
	r += (c >> 24) & 0xff;
	r += (d >> 24) & 0xff;
	r >>= 2;
	uint32_t g = 0;
	g += (a >> 16) & 0xff;
	g += (b >> 16) & 0xff;
	g += (c >> 16) & 0xff;
	g += (d >> 16) & 0xff;
	g >>= 2;
	uint32_t b_ = 0;
	b_ += (a >> 8) & 0xff;
	b_ += (b >> 8) & 0xff;
	b_ += (c >> 8) & 0xff;
	b_ += (d >> 8) & 0xff;
	b_ >>= 2;
	uint32_t a_ = 0;
	a_ += a & 0xff;
	a_ += b & 0xff;
	a_ += c & 0xff;
	a_ += d & 0xff;
	a_ >>= 2;
	return (r << 24) | (g << 16) | (b_ << 8) | a_;
}

int main(void) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_Window *window = SDL_CreateWindow(
		"Hello World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN
	);
	SDL_Surface *s = SDL_GetWindowSurface(window);
	char *buf = malloc(1920 * 1080 * 4);
	ss_init();
	SDL_Event e;
	uint64_t t = 0;
	uint32_t its = 0;
	while (true) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}
		ss_get_screenshot(buf);
		uint64_t tt = SDL_GetPerformanceCounter();
		for (int j = 0; j < 720; j++) {
			int sj1 = j + (j >> 1);
			int sj2 = sj1 + (j & 1);
			for (int i = 0; i < 1280; i++) {
				int si1 = i + (i >> 1);
				int si2 = si1 + (i & 1);
				((uint32_t *)s->pixels)[i + j * 1280] =
					avg(((uint32_t *)buf)[si1 + sj1 * 1920], ((uint32_t *)buf)[si2 + sj1 * 1920],
						((uint32_t *)buf)[si1 + sj2 * 1920], ((uint32_t *)buf)[si2 + sj2 * 1920]);
			}
		}
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
