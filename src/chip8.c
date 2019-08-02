#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "chip8.h"

void initializechip8(Chip8 *cpu) {
	cpu->opcode = 0;
	cpu->pc = 0x200;
	cpu->I = 0;
	cpu->sp = 0;
	cpu->drawflag = 1;
	cpu->soundtimer = 0;
	cpu->delaytimer = 0;

	memset(cpu->memory, 0, sizeof(cpu->memory));           // Clear memory
	memset(cpu->stack, 0, sizeof(cpu->stack));             // Clear stack
	memset(cpu->V, 0, sizeof(cpu->V));                     // Clear registers
	memset(cpu->screenstate, 0, sizeof(cpu->screenstate)); // Clear screen
	memset(cpu->key, 0, sizeof(cpu->key));                 // Clear key register


	// Load fontsets
	unsigned char chip8_fontset[80] =
		{
		 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		 0x20, 0x60, 0x20, 0x20, 0x70, // 1
		 0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		 0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		 0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		 0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		 0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		 0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		 0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		 0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		 0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		 0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		 0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		 0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		 0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		 0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		};
	// Loads basic fontset to memory from 0x50
	for (int i = 0; i < 80; ++i) {
		cpu->memory[i + 80] = chip8_fontset[i];
	}
}

int loadrom(Chip8 *cpu, char *romfile) {
	if (romfile == NULL) {
		printf("Error: No ROM selected in argv\n");
		return 1;
	}

	printf("Loading: %s\n", romfile);

	FILE *fileptr = fopen(romfile, "rb");

	// Get file size
	fseek(fileptr , 0 , SEEK_END);
	long lsize = ftell(fileptr);
	rewind(fileptr);
	printf("Filesize: %d\n", (int)lsize);

	// Allocate memory to fit whole file
	char *buffer = (char*)malloc(sizeof(char) *lsize);

	// Copy file to buffer
	fread(buffer, 1, lsize, fileptr);

	// Insert buffer to memory starting at 0x200
	for(int i = 0; i < lsize; ++i)
		cpu->memory[i + 512] = buffer[i];

	// Close file, free buffer
	fclose(fileptr);
	free(buffer);
	return 0;
}

void emulatecycle(Chip8 *cpu) {
	// Get opcode
	cpu->opcode = cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc + 1];

	// Decode opcode
	switch (cpu->opcode & 0xf000) {
	case 0x0000: {
		switch(cpu->opcode) {
		// 0x00e0 CLS - clear display
		case 0x00e0: {
			memset(cpu->screenstate, 0, sizeof(cpu->screenstate));
			cpu->pc += 2;
			break;
		}
		//0x00ee RET - return from a subroutine
		case 0x00ee: {
			cpu->sp--;
			// Pc +2 to skip function instruction it returns to lmao
			cpu->pc = cpu->stack[cpu->sp] + 2;
			break;
		}
		default: {
			printf("unknown instruction: %x\n", cpu->opcode);
			cpu->pc += 2;
			break;
		}
	}
		break;
	}

	// 1nnn JP addr - jump to location nnn
	case 0x1000: {
    cpu->pc = cpu->opcode & 0x0fff;
    break;
	}

	// 2nnn CALL addr - call subroutine at nnn
	case 0x2000: {
		cpu->stack[cpu->sp] = cpu->pc;
		cpu->sp++;
	 	cpu->pc = cpu->opcode & 0x0fff;
 		break;
	}

	// 3xkk SE Vx, byte - skip next instruction if Vx == kk
	case 0x3000: {
		cpu->pc += (cpu->V[(cpu->opcode & 0x0f00) >> 8] == (cpu->opcode & 0x00ff)) ? 4 : 2;
		break;
	}

	// 4xkk SNE Vx, byte - skip next instruction if Vx != kk
	case 0x4000: {
		cpu->pc += (cpu->V[(cpu->opcode & 0x0f00) >> 8] != (cpu->opcode & 0x00ff)) ? 4 : 2;
		break;
	}

	// 5xy0 SE Vx, Vy - skip next instruction if Vx == Vy
	case 0x5000: {
		cpu->pc += (cpu->V[(cpu->opcode & 0x0f00) >> 8] == cpu->V[(cpu->opcode & 0x00f0) >> 4])
			? 4 : 2;
		break;
	}

	// 6xkk LD Vx, byte - set Vx = kk
	case 0x6000: {
		cpu->V[(cpu->opcode & 0x0f00) >> 8] = cpu->opcode & 0x00ff;
		cpu->pc += 2;
		break;
	}

	// 7xkk ADD Vx, byte - set Vx = Vx + kk
	case 0x7000: {
		cpu->V[(cpu->opcode & 0x0f00) >> 8] += cpu->opcode & 0x00ff;
		cpu->pc += 2;
		break;
	}

	case 0x8000: {
		switch(cpu->opcode & 0x000f) {
		// 8xy0 LD Vx, Vy - set Vx = Vy
		case 0x0000: {
			cpu->V[(cpu->opcode & 0x0f00) >> 8] = cpu->V[(cpu->opcode & 0x00f0) >> 4];
			break;
		}
		// 8xy1 OR Vx, Vy - set Vx = Vx OR Vy
		case 0x0001: {
			cpu->V[(cpu->opcode & 0x0f00) >> 8] |= cpu->V[(cpu->opcode & 0x00f0) >> 4];
			break;
		}
		// 8xy2 AND Vx, Vy - set Vx = Vx AND Vy
		case 0x0002: {
			cpu->V[(cpu->opcode & 0x0f00) >> 8] &= cpu->V[(cpu->opcode & 0x00f0) >> 4];
			break;
		}
		// 8xy3 XOR Vx, Vy - set Vx = Vx XOR Vy
		case 0x003: {
			cpu->V[(cpu->opcode & 0x0f00) >> 8] ^= cpu->V[(cpu->opcode & 0x00f0) >> 4];
			break;
		}
		// 8xy4 ADD Vx, Vy - set Vx = Vx + Vy, set VF = carry
		case 0x0004: {
			cpu->V[0xf] = cpu->V[(cpu->opcode & 0x0f00) >> 8] + cpu->V[(cpu->opcode & 0x00f0) >> 4] > 255;
			cpu->V[(cpu->opcode & 0x0f00) >> 8] += cpu->V[(cpu->opcode & 0x00f0) >> 4];
			break;
		}
		// 8xy5 SUB Vx, Vy - set Vx = Vx - Vy, set VF = NOT borrow
		case 0x0005: {
			cpu->V[0xf] = (cpu->V[(cpu->opcode & 0x0f00) >> 8] > cpu->V[(cpu->opcode & 0x00f0) >> 4]);
			cpu->V[(cpu->opcode & 0x0f00) >> 8] -= cpu->V[(cpu->opcode & 0x00f0) >> 4];
			break;
		}
		// 8xy6 SHR Vx {, Vy} - set Vx = Vx SHR 1
		case 0x0006: {
			cpu->V[0xf] = cpu->V[(cpu->opcode & 0x0f00) >> 8] & 0x1;
			cpu->V[(cpu->opcode & 0x0f00) >> 8] >>= 1;
			break;
		}
		// 8xy7 SUBN Vx, Vy - set Vx = Vy - Vx, set VF = NOT borrow
		case 0x0007: {
			cpu->V[0xf] = cpu->V[(cpu->opcode & 0x00f0) >> 4] > cpu->V[(cpu->opcode & 0x0f00) >> 8];
			cpu->V[(cpu->opcode & 0x0f00) >> 8] = cpu->V[(cpu->opcode & 0x00f0) >> 4] -
				cpu->V[(cpu->opcode & 0x0f00) >> 8];
			break;
		}
 		// 8xye SHL Vx {, Vy} - set Vx = Vx SHL 1
		case 0x000e: {
			cpu->V[0xf] = (cpu->V[(cpu->opcode & 0x0f00) >> 8] & 0x80) >> 7;
			cpu->V[(cpu->opcode & 0x0f00) >> 8] <<= 1;
			break;
		}
		default: {
			printf("unknown instruction: %x\n", cpu->opcode);
			break;
		}
		}
		cpu->pc += 2;
		break;
	}

	// 9xy0 SNE Vx, Vy - skip next instruction if Vx != Vy
	case 0x9000: {
		cpu->pc += (cpu->V[(cpu->opcode & 0x0f00) >> 8] != cpu->V[(cpu->opcode & 0x00f0) >> 4])
			? 4 : 2;
		break;
	}

	// Annn LD I, addr - set I = nnn
	case 0xa000: {
    cpu->I = cpu->opcode & 0x0fff;
		cpu->pc += 2;
    break;
	}

	// Bnnn JP V0, addr - jump to location nnn + V0
	case 0xb000: {
    cpu->pc = (cpu->opcode & 0x0fff) + cpu->V[0];
    break;
	}

	// Cxkk RND Vx, byte - set Vx = random byte AND kk
	case 0xc000: {
    int r = rand() % 255;
		cpu->V[(cpu->opcode & 0x0f00) >> 8] = (cpu->opcode & 0x00ff) & r;
		cpu->pc += 2;
    break;
	}

	// Dxyn DRW Vx, Vy, nibble - Display n-byte sprite starting at memory location I at (Vx, Vy),
	// set Vf = collision
	case 0xd000: {
		int row;
		int x = (cpu->opcode & 0x0f00) >> 8;
		int y = (cpu->opcode & 0x00f0) >> 4;
		int ycord, xcord;
		cpu->V[0xf] = 0;
    for (int i = 0; i < (cpu->opcode & 0xf); i++) {
			row = cpu->memory[cpu->I + i];
			for (int n = 0; n < 8; n++) {
				// If bit is supposed to be drawn XOR it and check result for collision
				if (row & (0x80 >> n)) {
				  xcord = cpu->V[x] + n;
					ycord = cpu->V[y] + i;

					// Wrap cordinates outside of screen around
					if (ycord > 31) {
						ycord -= 32;
					}
					else if (ycord < 0) {
						ycord += 32;
					}
					if (xcord > 63) {
						xcord -= 64;
					}
					else if (xcord < 0) {
						xcord += 64;
					}
				  cpu->screenstate[xcord + 64*ycord] ^= 1;
					cpu->V[0xf] = (cpu->screenstate[xcord + 64*ycord] == 0);
				}
			}
		}
		cpu->drawflag = 1;
		cpu->pc += 2;
    break;
	}

	case 0xe000: {
		switch (cpu->opcode & 0x00ff) {
 		// Ex9e SKP Vx - skip next intruction if key with value of Vx is pressed
		case 0x9e: {
			cpu->pc += (cpu->key[cpu->V[(cpu->opcode & 0x0f00) >> 8]]) ? 4 : 2;
			break;
		}
			// Exa1 SKNP Vx - skip next intruction if key with value of Vx is not pressed
		case 0xa1: {
			cpu->pc += (cpu->key[cpu->V[(cpu->opcode & 0x0f00) >> 8]]) ? 2 : 4;
			break;
		}
		default: {
			printf("unknown instruction: %x\n", cpu->opcode);
			cpu->pc += 2;
			break;
		}
		}
    break;
	}

	case 0xf000: {
		switch(cpu->opcode & 0x00ff) {
		// Fx07 LD Vx, DT - set Vx = delaytimer
		case 0x07: {
			cpu->V[(cpu->opcode & 0x0f00) >> 8] = cpu->delaytimer;
			cpu->pc += 2;
		}
		// Fx0a LD Vx, K - wait for a key press, store the value of the key in Vx
		case 0x0a: {
			for (int i = 0; i < 16; ++i) {
				// If any key is pressed (==1) then store in Vx and increment pc otherwise do nothing
				if (cpu->key[i]) {
					cpu->V[(cpu->opcode & 0x0f00) >> 8] = i;
					cpu->pc += 2;
					break;
				}
			}
			break;
		}
		// Fx15 LD DT, Vx - set delay timer = Vx
		case 0x15: {
			cpu->delaytimer = cpu->V[(cpu->opcode & 0x0f00) >> 8];
			cpu->pc += 2;
			break;
		}

		// Fx18 LD ST, Vx - set soundtimer = Vx
		case 0x18: {
			cpu->soundtimer = cpu->V[(cpu->opcode & 0x0f00) >> 8];
			cpu->pc += 2;
			break;
		}
		// Fx1e ADD I, Vx - set I = I + Vx
		case 0x1e: {
			cpu->I += cpu->V[(cpu->opcode & 0x0f00) >> 8];
			cpu->pc += 2;
			break;
		}
		// Fx29 LD F, Vx - set I = location of sprite for digit Vx
		case 0x29: {
			// Fontset sprite corresponding to each number starts at 0x50 and each sprite is 5 bytes
			cpu->I = 5 * cpu->V[(cpu->opcode & 0x0f00) >> 8] + 0x50;
			cpu->pc += 2;
			break;
		}
		// Fx33 LD B, Vx - Store BCD representation of Vx in memory locations I, I+1, I+2
		case 0x33: {
			cpu->memory[cpu->I] = (cpu->V[(cpu->opcode & 0x0f00) >> 8]/100) % 10;
			cpu->memory[cpu->I + 1] = (cpu->V[(cpu->opcode & 0x0f00) >> 8]/10) % 10;
			cpu->memory[cpu->I + 2] = cpu->V[(cpu->opcode & 0x0f00) >> 8] % 10;
			cpu->pc += 2;
			break;
		}
		// Fx55 LD [I], Vx - Store registers V0 through Vx in memory starting at location I
		case 0x55: {
			for (int i = 0; i < (cpu->opcode & 0x0f00) >> 8; ++i) {
				cpu->memory[cpu->I + i] = cpu->V[i];
			}
			cpu->pc += 2;
			break;
		}
 		// Fx65 LD Vx, [I] - Read registers V0 through Vx from memory starting at location I
		case 0x65: {
			for (int i = 0; i < (cpu->opcode & 0x0f00) >> 8; ++i) {
				cpu->V[i] = cpu->memory[cpu->I + i];
			}
			cpu->pc += 2;
			break;
		}
		default: {
			printf("unknown instruction: %x\n", cpu->opcode);
			cpu->pc += 2;
			break;
		}
		}
		break;
	}
	default: {
		printf("unknown instruction: %x\n", cpu->opcode);
		cpu->pc += 2;
		break;
	}
	}

	// Update timers
	if (cpu->delaytimer > 0)
		--cpu->delaytimer;

	if (cpu->soundtimer > 0) {
		if (cpu->soundtimer == 1) {
			printf("SOUND");
		}
		--cpu->soundtimer;
	}
}
