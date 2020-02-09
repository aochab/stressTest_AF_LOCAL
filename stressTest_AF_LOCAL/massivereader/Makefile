CC=gcc
CFLAGS=-Wall
DEPS = massivereader.h
OBJ  = main.o massivereader.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

massivereader: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm *.o massivereader
