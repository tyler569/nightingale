# vim: noet ts=8 sw=8 sts=8

MAKEFILE	= Makefile

SOURCE_GLOB	= "[^_]*.[ch]"
ASM_GLOB	= "[^_]*.asm"

KERNEL_DIR 	= kernel
KERNEL		= $(KERNEL_DIR)/ngk
KERNEL_FILES	= $(shell find $(KERNEL_DIR)/ -type f -name $(SOURCE_GLOB)) \
		  $(shell find $(KERNEL_DIR)/ -type f -name $(ASM_GLOB))
LIBC_DIR	= libc
LIBC		= $(LIBC_DIR)/libc.a
LIBC_FILES	= $(shell find $(LIBC_DIR)/ -type f -name $(SOURCE_GLOB))
INIT_DIR	= user
INIT		= $(INIT_DIR)/initfs
INIT_FILES	= $(shell find $(INIT_DIR)/ -type f -name $(SOURCE_GLOB))

ISO		= ngos.iso

.PHONY: all clean iso remake

all: $(ISO)

%.asm:
	# stop circular dependancy "%: %.o"

$(KERNEL): $(KERNEL_FILES) $(MAKEFILE)
	$(MAKE) -C $(KERNEL_DIR)

$(LIBC): $(LIBC_FILES) $(MAKEFILE)
	$(MAKE) -C $(LIBC_DIR)

$(INIT): $(LIBC) $(INIT_FILES) $(MAKEFILE)
	$(MAKE) -C $(INIT_DIR)

clean:
	$(MAKE) -C $(KERNEL_DIR) clean
	$(MAKE) -C $(LIBC_DIR) clean
	$(MAKE) -C $(INIT_DIR) clean
	rm -rf build/*
	rm -f $(ISO)

$(ISO): $(KERNEL) kernel/grub.cfg $(INIT)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(KERNEL) isodir/boot
	cp $(INIT) isodir/boot
	grub-mkrescue -o $(ISO) isodir/
	rm -rf isodir

iso: $(ISO)

remake: clean $(ISO)

