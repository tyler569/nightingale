# vim: noet ts=8 sw=8 sts=8

.SUFFIXES:

MAKEFILE	= Makefile

SOURCE_GLOB	= "[^_]*.[ch]"
ASM_GLOB	= "[^_]*.asm"

GRUB_CFG	= kernel/grub.cfg

ifndef ARCH
ARCH		= X86_64
endif

ifeq ($(ARCH), X86_64)
ARCHDIR		= kernel/x86/64
else ifeq ($(ARCH), I686)
ARCHDIR		= kernel/x86/32
else
$(error ARCH $(ARCH) is not valid)
endif

include $(ARCHDIR)/arch.mk
include make-common.mk
export ARCH

KERNEL_BIN	= ngk
export KERNEL	:= $(BUILDDIR)/$(KERNEL_BIN)

LIBKNC_NAME	= libknc.a
export LIBKNC	:= $(BUILDDIR)/$(LIBKNC_NAME)

LIBC_NAME	= libc.a
export LIBC	:= $(BUILDDIR)/$(LIBC_NAME)

INITFS_NAME	= initfs
export INITFS	:= $(BUILDDIR)/$(INITFS_NAME)

LINKER_NAME	= liblinker.a
export LINKER	:= $(BUILDDIR)/$(LINKER_NAME)

all: iso

.PHONY: clean cleandep iso
clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C user clean
	$(MAKE) -C libc clean
	rm -rf buildI686
	rm -rf buildX86_64
	rm -f ngos32.iso ngos64.iso

cleandep:
	find . -name '*.d' | xargs rm

$(LIBKNC): $(shell find libc)
	$(Q)make NG=1 -C libc $(LIBKNC)

$(LIBC): $(shell find libc)
	$(Q)$(MAKE) -C libc

$(LINKER): $(shell find linker)
	$(Q)$(MAKE) CFLAGS="$(KCFLAGS)" -C linker klib

$(INITFS): $(shell find user) $(LIBC) 
	$(Q)$(MAKE) -C user

$(KERNEL): $(shell find kernel) $(LINKER) $(LIBKNC)
	$(Q)$(MAKE) -C kernel

$(ISO): $(KERNEL) $(INITFS) $(GRUB_CFG)
	@echo "ISO" $(notdir $(ISO))
	@mkdir -p isodir/boot/grub
	@cp $(GRUB_CFG) isodir/boot/grub
	@cp $(KERNEL) isodir/boot
	@cp $(INITFS) isodir/boot
	@grub-mkrescue -o $(ISO) isodir/ $(N)
	@rm -rf isodir

iso: $(ISO)

