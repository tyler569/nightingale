.PHONY: all clean

all: kernel/ngk.elf libc/libc.a

kernel/ngk.elf:
	$(MAKE) -C kernel

libc/libc.a:
	$(MAKE) -C libc

clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C libc clean
