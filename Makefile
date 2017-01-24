
TARGET  = nightingale.kernel
ISO		= nighitngale.iso

CC = i686-elf-gcc
AS = $(CC)
LD = $(CC)

CFLAGS  = -Iinclude -ffreestanding -Wall -Wpedantic -std=gnu11 -nostdlib -g -c
ASFLAGS = -ffreestanding -Wall -Wpedantic -nostdlib -g -c
LDFLAGS = -nostdlib -Tkernel/link.ld

SRCDIR		= kernel

CSOURCES	:= $(wildcard $(SRCDIR)/*.c)
ASOURCES	:= $(wildcard $(SRCDIR)/*.S)
COBJECTS	:= $(CSOURCES:$(SRCDIR)/%.c=$(SRCDIR)/%.o)
AOBJECTS	:= $(ASOURCES:$(SRCDIR)/%.S=$(SRCDIR)/%.o)
OBJECTS		:= $(AOBJECTS) $(COBJECTS)

.PHONY: all clean iso run debug

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECTS) -lgcc

$(COBJECTS): $(SRCDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(AOBJECTS): $(SRCDIR)/%.o : $(SRCDIR)/%.S
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(SRCDIR)/*.o
	rm -f $(TARGET)
	rm -f $(ISO)

iso: $(TARGET)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(TARGET) isodir/boot
	grub-mkrescue -o $(ISO) isodir/
	rm -rf isodir

run: iso
	qemu-system-i386 -curses -cdrom $(ISO)

debug: iso
	qemu-system-i386 -curses -cdrom $(ISO) -S -s
