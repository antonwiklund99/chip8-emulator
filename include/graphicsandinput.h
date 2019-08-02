#ifndef GRAPHICSANDINPUT_GUARD
#define GRAPHICSANDINPUT_GUARD

#include <SDL2/SDL.h>
#include "chip8.h"

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
} Graphicshandler;

void setupgraphicshandler(Graphicshandler *ppu);
void drawgraphics(Graphicshandler *ppu, Chip8 *cpu);
void cleanupgraphicshandler(Graphicshandler *ppu);

int storekeys(Chip8 *cpu);

#endif
