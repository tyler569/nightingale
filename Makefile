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

ALL_FILES	= $(shell find . -type f -name $(SOURCE_GLOB)) \
		  $(shell find . -type f -name $(ASM_GLOB))

ISO32		= ngos32.iso
ISO64		= ngos64.iso

BUILD64_DIR	= buildX86_64
BUILD32_DIR	= buildI686

KERNEL_DIR	= kernel
KERNEL		= ngk
KERNEL64	= $(BUILD64_DIR)/$(KERNEL)
KERNEL32	= $(BUILD32_DIR)/$(KERNEL)

USER_DIR	= user
INITFS		= initfs
INITFS64	= $(BUILD64_DIR)/$(USER_DIR)/$(INITFS)
INITFS32	= $(BUILD32_DIR)/$(USER_DIR)/$(INITFS)

.PHONY: all clean iso64 iso32 remake

all: iso64

clean:
	$(MAKE) -C $(KERNEL_DIR) clean
	$(MAKE) -C $(USER_DIR) clean
	rm -rf buildI686
	rm -rf buildX86_64
	rm -f $(ISO32) $(ISO64)

$(ISO64): kernel/grub.cfg $(ALL_FILES)
	$(MAKE) CC='$(XCC64)' AS='$(XAS64)' LD='$(XLD64)' ARCH=X86_64 -C $(KERNEL_DIR)
	$(MAKE) CC='$(XCC64)' AS='$(XAS64)' LD='$(XLD64)' ARCH=X86_64 -C $(USER_DIR)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(KERNEL64) isodir/boot
	cp $(INITFS64) isodir/boot
	grub-mkrescue -o $(ISO64) isodir/
	rm -rf isodir

iso64: $(ISO64)

$(ISO32): kernel/grub.cfg $(ALL_FILES)
	$(MAKE) CC='$(XCC32)' AS='$(XAS32)' LD='$(XLD32)' ARCH=I686 -C $(KERNEL_DIR)
	$(MAKE) CC='$(XCC32)' AS='$(XAS32)' LD='$(XLD32)' ARCH=I686 -C $(USER_DIR)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(KERNEL32) isodir/boot
	cp $(INITFS32) isodir/boot
	grub-mkrescue -o $(ISO32) isodir/
	rm -rf isodir

iso32: $(ISO32)

both: iso32 iso64

cibuild:
	# TODO will be used by travis, ARCH passed in from environment
	$(MAKE) CC='$(XCC32)' AS='$(XAS32)' LD='$(XLD32)' -C $(KERNEL_DIR)
	$(MAKE) CC='$(XCC32)' AS='$(XAS32)' LD='$(XLD32)' -C $(USER_DIR)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(KERNEL32) isodir/boot
	cp $(INITFS32) isodir/boot
	grub-mkrescue -o $(ISO32) isodir/
	rm -rf isodir

