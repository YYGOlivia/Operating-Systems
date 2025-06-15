CC=gcc
CFLAGS=-Wall
DOXYGEN=doxygen
DOXYGEN_CONFIG=Doxyfile

all: test

test: main.o test.o
	$(CC) $(CFLAGS) -o test main.o test.o

main.o: main.c test.h
	$(CC) $(CFLAGS) -c main.c

test.o: test.c test.h
	$(CC) $(CFLAGS) -c test.c

doc:
	$(DOXYGEN) $(DOXYGEN_CONFIG)

clean:
	rm -f *.o test
