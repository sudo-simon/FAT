CC = gcc
CCOPTS = -Wall -std=gnu99 -g
AR = ar

BINS = fat.bin
OBJS = ./cli/shell.o ./cli/commands.o
HEADERS = constants.h ./cli/shell.h ./cli/commands.h
LIBS = 

%.o: %.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@ $<

.phony: clean all

fat.bin: main.c $(OBJS)
	$(CC) $(CCOPTS) -O3 $^ -lreadline -o $@

clean:
	find . -name \*.o -type f -exec rm -rf {} +
	find . -name \*~ -type f -exec rm -rf {} +
	find . -name \*.a -type f -exec rm -rf {} +
	find . -name fat.bin -type f -exec rm -rf {} +
