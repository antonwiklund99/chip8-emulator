#include <stdio.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "graphicsandinput.h"

int main(int argc, char *argv[])
{
	// Setup graphics
	Graphicshandler ppu;
	setupgraphicshandler(&ppu);

	// Init chip8
	Chip8 cpu;
	initializechip8(&cpu);

	// Load rom to memory
	if (loadrom(&cpu, argv[1]) != 0) {
		return 1;
	}

	int quit = 0;
	while (quit == 0) {
		emulatecycle(&cpu);

		if (cpu.drawflag) {
			drawgraphics(&ppu, &cpu);
		}

		// Store keypresses and return 1 if window close is pressed
		if (storekeys(&cpu)) {
			cleanupgraphicshandler(&ppu);
			quit = 1;
		}

		// Clock is 60hz
		SDL_Delay(1000/60);
	}
	return 0;
}
