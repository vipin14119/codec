CC=gcc
CFLAGS=-Wall -g -DDEBUG

all: main.c codec.c directory.c node.c hlinks.c
	$(CC) $(CFLAGS) codec.c directory.c node.c hlinks.c main.c -o codec

test: codec
	./codec -e encodetest02 testdir

cleandir: testdir
	rm -rf testdir

clean: codec
	rm codec 
