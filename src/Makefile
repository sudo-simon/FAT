CC = gcc
CCOPTS = -Wall -std=gnu99 -g
AR = ar

BINS = fat.bin
OBJS = ./libs/linenoise/linenoise.o ./cli/shell_linenoise.o ./cli/commands.o ./fs/aux.o ./fs/disk.o ./fs/fat.o ./fs/file.o ./fs/functions.o ./editor/kilo.o
HEADERS = constants.h ./libs/linenoise/linenoise.h ./cli/shell_linenoise.h ./cli/commands.h ./fs/aux.h ./fs/disk.h ./fs/fat.h ./fs/file.h ./fs/functions.h ./editor/kilo.h
LIBS = libFAT.a

%.o: %.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@ $<


.phony: clean all


all: $(BINS) $(LIBS)


libFAT.a: $(OBJS)
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)


fat.bin: main.c $(OBJS)
	$(CC) $(CCOPTS) -O3 $^ -o $@




clean:
	find . -name \*.o -type f -exec rm -rf {} +
	find . -name \*~ -type f -exec rm -rf {} +
	find . -name \*.a -type f -exec rm -rf {} +
	find . -name fat.bin -type f -exec rm -rf {} +
