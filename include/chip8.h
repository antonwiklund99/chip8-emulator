#ifndef CHIP8_GUARD
#define CHIP8_GUARD

typedef struct {
  // Current opcode
  unsigned short opcode;

  // RAM
  unsigned char memory[4096];

  // Registers V0-VF and I
  unsigned char V[16];
  unsigned short I;

  // Program counter starts at 0x200 (512)
  unsigned short pc;

  // Stack and stack pointer
  unsigned short stack[16];
  unsigned char sp;

  // Delay and sound timers
  unsigned char soundtimer;
  unsigned char delaytimer;

  // Graphics (64x32 pixels)
  unsigned char screenstate[64 * 32];
  // Indicates if screen needs to be updated
  unsigned char drawflag;

  // Keypad, stores current state of keys
  unsigned char key[16];
} Chip8;

void initializechip8(Chip8 *cpu);
int loadrom(Chip8 *cpu, char *romfile);
void emulatecycle(Chip8 *cpu);

#endif
