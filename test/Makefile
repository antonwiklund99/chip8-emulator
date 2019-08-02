test: unittests.c ../src/chip8.c ../include/chip8.h
	gcc -Wall -I../include unittests.c ../src/chip8.c -lcunit -o test

.PHONY: clean

clean:
	rm -f *~ test
