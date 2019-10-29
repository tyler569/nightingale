# vim: noet ts=8 sw=8 sts=8

.SUFFIXES:

MAKEFILE	= Makefile

SOURCE_GLOB	= "[^_]*.[ch]"
ASM_GLOB	= "[^_]*.asm"

GRUB_CFG	= kernel/grub.cfg

ALL_FILES	= $(shell find . -type f -name $(SOURCE_GLOB)) \
		  $(shell find . -type f -name $(ASM_GLOB)) \
		  $(MAKEFILE) $(GRUB_CFG)

ifndef ARCH
ARCH		= X86_64
endif

ifeq ($(ARCH), X86_64)
include kernel/x86/64/make-X86_64.mk
else ifeq ($(ARCH), I686)
include kernel/x86/32/make-I686.mk
else
$(error ARCH $(ARCH) is not valid)
endif

include make-common.mk
export ARCH

KERNEL_BIN	= ngk
KERNEL		= $(BUILDDIR)/$(KERNEL_BIN)

USER_DIR	= user
INITFS_NAME	= initfs
INITFS		= $(BUILDDIR)/$(USER_DIR)/$(INITFS_NAME)

all: iso

clean:
	rm -rf buildI686
	rm -rf buildX86_64
	rm -f ngos32.iso ngos64.iso
	make -C kernel clean
	make -C user clean

$(INITFS): $(shell find user incldue)
	$(Q)make -C user

$(KERNEL): $(shell find kernel include)
	$(Q)make -C kernel

$(ISO): $(KERNEL) $(INITFS)
	@mkdir -p isodir/boot/grub
	@cp kernel/grub.cfg isodir/boot/grub
	@cp $(KERNEL) isodir/boot
	@cp $(INITFS) isodir/boot
	@grub-mkrescue -o $(ISO) isodir/ $(N)
	@rm -rf isodir
	@echo "ISO" $(notdir $(ISO))

test_runner: test_runner.c
	gcc -g -Wall test_runner.c -o test_runner

iso: $(ISO)

