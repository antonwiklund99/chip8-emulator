#include "chip8.h"
#include "graphicsandinput.h"
#include <SDL2/SDL.h>

int storekeys(Chip8 *cpu) {
	SDL_Event event;
	int eventvalue; // Value representing keydown or up - down = 1 up = 0
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN) {
			eventvalue = (event.type == SDL_KEYDOWN);
			switch (event.key.keysym.sym) {
			case SDLK_1: cpu->key[0] = eventvalue; break;
			case SDLK_2: cpu->key[1] = eventvalue; break;
			case SDLK_3: cpu->key[2] = eventvalue; break;
			case SDLK_4: cpu->key[3] = eventvalue; break;
			case SDLK_q: cpu->key[4] = eventvalue; break;
			case SDLK_w: cpu->key[5] = eventvalue; break;
			case SDLK_e: cpu->key[6] = eventvalue; break;
			case SDLK_r: cpu->key[7] = eventvalue; break;
			case SDLK_a: cpu->key[8] = eventvalue; break;
			case SDLK_s: cpu->key[9] = eventvalue; break;
			case SDLK_d: cpu->key[10] = eventvalue; break;
			case SDLK_f: cpu->key[11] = eventvalue; break;
			case SDLK_z: cpu->key[12] = eventvalue; break;
			case SDLK_x: cpu->key[13] = eventvalue; break;
			case SDLK_c: cpu->key[14] = eventvalue; break;
			case SDLK_v: cpu->key[15] = eventvalue; break;
			default: break;
			}
		}
		else if (event.type == SDL_QUIT) {
			return 1;
		}
	}
	return 0;
}
