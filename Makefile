.PHONY: default all clean test
default: test
#default: all

all: parser

CC=gcc

clean:
	-rm *.o
	-rm parser

test: parser
	./parser < parser.c

parser: scanner.o parser.o
	$(CC) -o parser scanner.o parser.o

scanner.o: scanner.c
	$(CC) -o scanner.o -c scanner.c

parser.o: parser.c
	$(CC) -o parser.o -c parser.c


