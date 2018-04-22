# vim: noet ts=8 sw=8 sts=8

KERNEL_DIR 	= kernel
CSRC		= $(shell find $(KERNEL_DIR) -name "[^_]*.[ch]")
ASRC		= $(shell find $(KERNEL_DIR) -name "[^_]*.asm")

KERNEL		= kernel/ngk
KERNEL_FILE	= $(CSRC) $(CHDR) $(ASRC)
LIBC		= libc/libc.a
LIBC_FILES	= $(shell find libc/ -name "[^_]*.[ch]")
INIT		= user/test_user
INIT_FILES	= user/test_user.c

ISO		= ngos.iso

.PHONY: all clean iso dump dumps dump32

all: $(ISO)

%.asm:
	# stop circular dep "%: %.o"

$(KERNEL): $(KERNEL_FILES) $(MAKEFILE)
	$(MAKE) -C $(KERNEL_DIR)

$(LIBC): $(LIBC_FILES) $(MAKEFILE)
	$(MAKE) -C libc

$(INIT): $(LIBC) $(INIT_FILES) $(MAKEFILE)
	$(MAKE) -C user

clean:
	rm -f $(shell find . -name "*.o")
	rm -f $(shell find . -name "*.d")
	rm -f $(KERNEL)
	rm -f $(LIBC)
	rm -f $(INIT)
	rm -f $(ISO)

$(ISO): $(KERNEL) kernel/grub.cfg $(INIT)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(KERNEL) isodir/boot
	cp user/test_user isodir/boot
	grub-mkrescue -o $(ISO) isodir/
	rm -rf isodir

iso: $(ISO)

dump: $(KERNEL)
	x86_64-elf-objdump -Mintel -d $(KERNEL) | less

dumps: $(KERNEL)
	x86_64-elf-objdump -Mintel -dS -j.text -j.low.text $(KERNEL) | less

dump32: $(KERNEL)
	x86_64-elf-objdump -Mintel,i386 -d $(KERNEL) | less

dump32s :$(KERNEL)
	x86_64-elf-objdump -Mintel,i386 -dS $(KERNEL) | less

