# vim: noet ts=8 sw=8 sts=8

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
include make-X86_64.mk
else ifeq ($(ARCH), I686)
include make-I686.mk
else
$(error ARCH $(ARCH) is not valid)
endif

include make-common.mk
export ARCH

KERNEL_BIN	= ngk
KERNEL		= $(BUILDDIR)/$(KERNEL_BIN)

KERNEL_MODULES  = arch \
		  ds \
		  fs \
		  drv \
		  net \
		  kernel

KERNEL_LIBS	= lib$(ARCH).a \
		  libds.a \
		  libfs.a \
		  libdrv.a \
		  libnet.a \
		  libnightingale.a

KERNEL_LIBS_F	= $(addprefix $(BUILDDIR)/, $(KERNEL_LIBS))

USER_DIR	= user
INITFS_NAME	= initfs
INITFS		= $(BUILDDIR)/$(USER_DIR)/$(INITFS_NAME)

all: iso

clean:
	rm -rf buildI686
	rm -rf buildX86_64
	rm -f ngos32.iso ngos64.iso

$(BUILDDIR)/libnightingale.a: $(shell find kernel)
	make -C kernel

$(BUILDDIR)/libfs.a: $(shell find fs)
	make -C fs

$(BUILDDIR)/libds.a: $(shell find ds)
	make -C ds

$(BUILDDIR)/lib$(ARCH).a: $(shell find arch)
	make -C arch

$(BUILDDIR)/libdrv.a: $(shell find drv)
	make -C drv

$(BUILDDIR)/libnet.a: $(shell find net)
	make -C net

$(INITFS): $(shell find user)
	make -C user

$(KERNEL): $(KERNEL_LIBS_F) $(INITFS)
	$(LD) $(KLDFLAGS) -o $(KERNEL) -Wl,--start-group $(KERNEL_LIBS_F) -Wl,--end-group -lgcc

$(ISO): $(KERNEL) $(INITFS)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(KERNEL) isodir/boot
	cp $(INITFS) isodir/boot
	grub-mkrescue -o $(ISO) isodir/
	rm -rf isodir

iso: $(ISO)

