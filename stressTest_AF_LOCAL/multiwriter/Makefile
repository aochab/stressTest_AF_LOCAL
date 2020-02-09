CC=gcc
CFLAGS=-Wall
DEPS = multiwriter.h
OBJ  = main.o multiwriter.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

serwer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm *.o multiwriter
