#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "chip8.h"

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

Chip8 testcpu;

void callandrettest(void) {
	initializechip8(&testcpu);

	// Call subroutine test, calls subroutine at 0x250
	testcpu.memory[512] = 0x22;
	testcpu.memory[513] = 0x50;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.pc == 0x250);
	CU_ASSERT(testcpu.sp == 1);
	CU_ASSERT(testcpu.stack[0] == 512);

	// Call another subroutine at 0x275
	testcpu.memory[0x250] = 0x22;
	testcpu.memory[0x250 + 1] = 0x75;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.pc == 0x275);
	CU_ASSERT(testcpu.sp == 2);
	CU_ASSERT(testcpu.stack[1] == 0x250);

	// Return from subroutine
	testcpu.memory[0x275] = 0x00;
	testcpu.memory[0x275 + 1] = 0xee;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.pc == 0x252);
	CU_ASSERT(testcpu.sp == 1);

	// Return again
	testcpu.memory[0x252] = 0x00;
	testcpu.memory[0x252 + 1] = 0xee;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.pc == 514);
	CU_ASSERT(testcpu.sp == 0);
}

void skiptest(void) {
	initializechip8(&testcpu);

	// Skips instruction if Vx == kk
	testcpu.V[5] = 8;
	testcpu.memory[512] = 0x35;
	testcpu.memory[513] = 0x08;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.pc == 516);

	// Skips instruction if Vx != kk
	testcpu.memory[512] = 0x45;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.pc == 514);

	// Skips instruction if Vx == Vy
	testcpu.memory[512] = 0x55;
	testcpu.memory[513] = 0x70;
	testcpu.pc = 512;
	testcpu.V[7] = 12;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.pc == 514);

	// Skips instruction if Vx != Vy
	testcpu.memory[512] = 0x95;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.pc == 516);
}

void regtest(void) {
	initializechip8(&testcpu);

	// Puts value into register
	testcpu.memory[512] = 0x6a;
	testcpu.memory[513] = 0xee;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == 0xee);
	CU_ASSERT(testcpu.pc == 514);

	// Adds kk to reg Vx
	testcpu.memory[512] = 0x7a;
	testcpu.V[0xa] = 0x10;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == 0xfe);
	CU_ASSERT(testcpu.pc == 514);

	// Store value of Vy in Vx
	testcpu.memory[512] = 0x8a;
	testcpu.memory[513] = 0xb0;
	testcpu.V[0xb] = 3;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == 3);
	CU_ASSERT(testcpu.pc == 514);

	//Vx = Vx OR Vy
	testcpu.memory[513] = 0xb1;
	testcpu.V[0xb] = 0xaa;
	testcpu.V[0xa] = 0x09;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == (0xaa | 0x09));
	CU_ASSERT(testcpu.pc == 514);

	//Vx = Vx AND Vy
	testcpu.memory[513] = 0xb2;
	testcpu.V[0xb] = 0xaa;
	testcpu.V[0xa] = 0x09;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == (0xaa & 0x09));
	CU_ASSERT(testcpu.pc == 514);

	//Vx = Vx XOR Vy
	testcpu.memory[513] = 0xb3;
	testcpu.V[0xb] = 0xaa;
	testcpu.V[0xa] = 0x09;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == (0xaa ^ 0x09));
	CU_ASSERT(testcpu.pc == 514);

	// ADD
	testcpu.memory[513] = 0xb4;
	testcpu.V[0xb] = 0xaa;
	testcpu.V[0xa] = 0x09;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == (0xaa + 0x09));
	CU_ASSERT(testcpu.V[0xf] == 0);
	CU_ASSERT(testcpu.pc == 514);

	// ADD with value larger than 8-bit
	testcpu.V[0xa] = 0xfe;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == 0xa8);
	CU_ASSERT(testcpu.V[0xf] == 1);

	// SUB
	testcpu.memory[513] = 0xb5;
	testcpu.V[0xb] = 0x09;
	testcpu.V[0xa] = 0xaa;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == (0xaa - 0x09));
	CU_ASSERT(testcpu.V[0xf] == 1);
	CU_ASSERT(testcpu.pc == 514);

	// SUB with value larger than Vx
	testcpu.V[0xa] = 0x02;
	testcpu.pc = 512;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xf] == 0);
	CU_ASSERT(testcpu.pc == 514);

	// SHR
	testcpu.V[0xa] = 3;
	testcpu.pc = 512;
	testcpu.memory[513] = 0xb6;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == 3 / 2);
	CU_ASSERT(testcpu.V[0xf] == 1);
	CU_ASSERT(testcpu.pc == 514);

	testcpu.V[0xa] = 2;
	testcpu.pc = 512;
	testcpu.memory[513] = 0xb6;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == 1);
	CU_ASSERT(testcpu.V[0xf] == 0);
	CU_ASSERT(testcpu.pc == 514);

	//TODO SUBN

	//SHL
	testcpu.V[0xa] = 0xaa;
	testcpu.pc = 512;
	testcpu.memory[513] = 0xbe;
	emulatecycle(&testcpu);
	CU_ASSERT(testcpu.V[0xa] == 0x54);
	CU_ASSERT(testcpu.V[0xf] == 1);


}

int main() {
	// Init test registry
	if (CUE_SUCCESS != CU_initialize_registry()) {
		return CU_get_error();
	}

	// Add suites to test registry
	CU_pSuite opcodesuite = NULL;
	opcodesuite = CU_add_suite("opcode suite", init_suite, clean_suite);
	if (opcodesuite == NULL) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	// Add tests to suite
	if (CU_add_test(opcodesuite, "\n\n.....Testing calling and returning from subroutine.....\n\n",
									callandrettest) == NULL) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if (CU_add_test(opcodesuite, "\n\n.....Testing skip next instructions.....\n\n",
									skiptest) == NULL) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if (CU_add_test(opcodesuite, "\n\n.....Testing reg operations.....\n\n",
									regtest) == NULL) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	// Run tests
	CU_basic_run_tests();

	CU_cleanup_registry();
	return CU_get_error();
}
