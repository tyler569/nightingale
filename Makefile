# vim: noet ts=8 sw=8 sts=8

MAKEFILE	= Makefile

XCC64		= x86_64-elf-gcc
XAS64		= nasm -felf64
XLD64		= x86_64-elf-gcc

XCC32		= i686-elf-gcc
XAS32		= nasm -felf32
XLD32		= i686-elf-gcc

SOURCE_GLOB	= "[^_]*.[ch]"
ASM_GLOB	= "[^_]*.asm"

KERNEL_DIR 	= kernel
KERNEL32	= $(KERNEL_DIR)/ngk32
KERNEL64	= $(KERNEL_DIR)/ngk64
KERNEL_FILES	= $(shell find $(KERNEL_DIR)/ -type f -name $(SOURCE_GLOB)) \
		  $(shell find $(KERNEL_DIR)/ -type f -name $(ASM_GLOB))
LIBC_DIR	= libc
LIBC		= $(LIBC_DIR)/libc.a
LIBC_FILES	= $(shell find $(LIBC_DIR)/ -type f -name $(SOURCE_GLOB))

INIT_DIR	= user
INIT		= $(INIT_DIR)/initfs
INIT_FILES	= $(shell find $(INIT_DIR)/ -type f -name $(SOURCE_GLOB))

ISO32		= ngos32.iso
ISO64		= ngos64.iso

.PHONY: all clean iso64 iso32 remake

all: $(ISO64)

%.asm:
	# stop circular dependancy "%: %.o"


clean:
	$(MAKE) -C $(KERNEL_DIR) clean
	$(MAKE) -C $(LIBC_DIR) clean
	$(MAKE) -C $(INIT_DIR) clean
	rm -rf build/*
	rm -f $(ISO)

$(ISO64): kernel/grub.cfg
	$(MAKE) CC='$(XCC64)' AS='$(XAS64)' LD='$(XLD64)' TARGET=64 -C $(KERNEL_DIR)
	$(MAKE) CC='$(XCC64)' AS='$(XAS64)' LD='$(XLD64)' TARGET=64 -C $(LIBC_DIR)
	$(MAKE) CC='$(XCC64)' AS='$(XAS64)' LD='$(XLD64)' TARGET=64 -C $(INIT_DIR)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(KERNEL64) isodir/boot
	cp $(INIT64) isodir/boot
	grub-mkrescue -o $(ISO64) isodir/
	rm -rf isodir

iso64: $(ISO64)

$(ISO32): $(KERNEL32) kernel/grub.cfg $(INIT32)
	$(MAKE) CC=$(XCC32) AS=$(XAS32) LD=$(XLD32) TARGET=32 -C $(KERNEL_DIR)
	$(MAKE) CC=$(XCC32) AS=$(XAS32) LD=$(XLD32) TARGET=32 -C $(LIBC_DIR)
	$(MAKE) CC=$(XCC32) AS=$(XAS32) LD=$(XLD32) TARGET=32 -C $(INIT_DIR)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(KERNEL32) isodir/boot
	cp $(INIT32) isodir/boot
	grub-mkrescue -o $(ISO32) isodir/
	rm -rf isodir

iso32: $(ISO32)

remake: clean $(ISO64)

