# vim: noet ts=8 sts=8 sw=8

CC = x86_64-elf-gcc

CFLAGS = -I../include -ffreestanding -nostdlib

INIT_FILES = init test

all:
	$(CC) $(CFLAGS) init.c -oinit -I../libc -L../libc -lc
	$(CC) $(CFLAGS) test.c -otest -I../libc -L../libc -lc

	tar cf initfs.tar $(INIT_FILES)

