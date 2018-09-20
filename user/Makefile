# vim: noet ts=8 sts=8 sw=8

CC		= x86_64-elf-gcc

CFLAGS		=  -O0
override CFLAGS	:= $(CFLAGS) -g -I../libc -I../include -Wall -ffreestanding -nostdlib -L../libc

INIT_FILES	= init test echo bf net rsh sh bomb uname

.PHONY: all

all: initfs
 
%: %.c ../libc/libc.a
	$(CC) $(CFLAGS) $< -o $@ -lc

initfs: $(INIT_FILES)
	tar cf initfs $(INIT_FILES) text_file

clean:
	rm -f *.o $(INIT_FILES)
	rm -f initfs

