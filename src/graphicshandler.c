#include "graphicsandinput.h"
#include "chip8.h"
#include <stdio.h>
#include <SDL2/SDL.h>

#define PIXELSIZE 10

void setupgraphicshandler(Graphicshandler *ppu) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("ERROR initializing SDL: %s\n", SDL_GetError());
	}
	ppu->window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
																 64*PIXELSIZE, 32*PIXELSIZE, 0);
	ppu->renderer = SDL_CreateRenderer(ppu->window, -1, 0);
}

void drawgraphics(Graphicshandler *ppu, Chip8 *cpu) {
	SDL_RenderClear(ppu->renderer);
	SDL_SetRenderDrawColor(ppu->renderer, 0xff, 0xff, 0xff, 0xff);

	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 64; x++) {
			if (cpu->screenstate[x + (64*y)]) {
			  SDL_Rect r = {.x = x*PIXELSIZE, .y = y*PIXELSIZE,
											.w = PIXELSIZE, .h = PIXELSIZE};
				SDL_RenderDrawRect(ppu->renderer, &r);
				SDL_RenderFillRect(ppu->renderer, &r);
			}
		}
	}

	SDL_SetRenderDrawColor(ppu->renderer, 0, 0, 0, 0);
	SDL_RenderPresent(ppu->renderer);
	cpu->drawflag = 0;
}

void cleanupgraphicshandler(Graphicshandler *ppu) {
	SDL_DestroyRenderer(ppu->renderer);
	SDL_DestroyWindow(ppu->window);
	SDL_Quit();
}
