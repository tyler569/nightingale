# vim: noet ts=8 sts=8 sw=8

CC = x86_64-elf-gcc

CFLAGS = -I../include -ffreestanding -nostdlib

INIT_FILES = init test text_file

all:
	$(CC) $(CFLAGS) init.c -oinit -I../libc -L../libc -lc
	$(CC) $(CFLAGS) test.c -otest -I../libc -L../libc -lc

	tar cf initfs $(INIT_FILES)

clean:
	rf -f *.o
	rf -f init
	rf -f test
	rf -f initfs

