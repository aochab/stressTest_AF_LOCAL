CC=gcc
CFLAGS=-Wall -lm
DEPS = multiwriter.h
OBJ  = main.o multiwriter.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

multiwriter: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm *.o multiwriter
