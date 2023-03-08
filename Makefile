CC = gcc
CFLAGS = -g -std=c99

all: sri_memory_reader

sri_memory_reader: sri_memory_reader.c component.h component.o
	$(CC) $(CFLAGS) -o $@ sri_memory_reader.c component.o

clean:
	rm -f sri_memory_reader