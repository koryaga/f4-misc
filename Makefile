CC=gcc
CFLAGS=-I. -std=gnu11
#using gnu11 for strndup and for(int i loop 

SOURCES=libf4.c f4cli.c libcli.c
OBJ=$(SOURCES:.c=.o)

all: f4cli 

f4cli: f4cli.o libf4.o libcli.o
	$(CC) $(CFLAGS)  $(OBJ) -o $@

test: 
	rm -rf libf4.o
	$(CC) $(CFLAGS) -DTEST libf4.c -o f4_$@
	./f4_$@

clean:
	rm -rf $(OBJ)  f4_test f4cli
