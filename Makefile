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
	$(Q)make -C kernel

$(BUILDDIR)/libfs.a: $(shell find fs)
	$(Q)make -C fs

$(BUILDDIR)/libds.a: $(shell find ds)
	$(Q)make -C ds

$(BUILDDIR)/lib$(ARCH).a: $(shell find arch)
	$(Q)make -C arch

$(BUILDDIR)/libdrv.a: $(shell find drv)
	$(Q)make -C drv

$(BUILDDIR)/libnet.a: $(shell find net)
	$(Q)make -C net

$(INITFS): $(shell find user)
	$(Q)make -C user

$(KERNEL): $(KERNEL_LIBS_F)
	$(Q)$(LD) $(KLDFLAGS) -o $(KERNEL) -Wl,--start-group $(KERNEL_LIBS_F) -Wl,--end-group -lgcc
	@echo "LINK" $(notdir $(KERNEL))

$(ISO): $(KERNEL) $(INITFS)
	@mkdir -p isodir/boot/grub
	@cp kernel/grub.cfg isodir/boot/grub
	@cp $(KERNEL) isodir/boot
	@cp $(INITFS) isodir/boot
	@grub-mkrescue -o $(ISO) isodir/ $(N)
	@rm -rf isodir
	@echo "ISO" $(notdir $(ISO))

iso: $(ISO)

