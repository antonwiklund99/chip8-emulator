IDIR=include
SDIR=src
ODIR=src/obj

CC=gcc
CFLAGS=-Wall -I$(IDIR)

_DEPS=chip8.h graphicsandinput.h
DEPS=$(patsubst %, $(IDIR)/%, $(_DEPS))

_OBJ=chip8.o emulator.o graphicshandler.o inputhandler.o
OBJ=$(ODIR)/chip8.o $(ODIR)/emulator.o $(ODIR)/graphicshandler.o $(ODIR)/inputhandler.o

LIBS=-lSDL2

$(ODIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
emulator: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(SDIR)/*~ $(IDIR)/*~ emulator
