CC=gcc
CFLAGS=-I.

SOURCES=libf4.c f4cli.c
OBJ=$(SOURCES:.c=.o)

all: f4cli lib

f4cli: f4cli.o lib
	$(CC) $(CFLAGS)  $(OBJ) -o $@

lib: libf4.o f4.h

test: 
	$(CC) $(CFLAGS) -DTEST $(SOURCES) -o f4_$@

clean:
	rm -rf $(OBJ)  f4_test f4cli
