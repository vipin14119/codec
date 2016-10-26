CC=gcc
CFLAGS=-Wall -g

all: main.c codec.o directory.o node.o hlinks.o
	$(CC) $(CFLAGS) -o codec  codec.o directory.o node.o hlinks.o main.c 

test: codec
	./codec -e encodetest02 testdir

codec.o: codec.c codec.h
	$(CC) $(CFLAGS) -c $<

directory.o: directory.c directory.h
	$(CC) $(CFLAGS) -c $<

node.o: node.c node.h
	$(CC) $(CFLAGS) -c $<

hlinks.o: hlinks.c hlinks.h
	$(CC) $(CFLAGS) -c $<

cleandir: testdir
	rm -rf testdir

clean: codec
	rm codec *.o
