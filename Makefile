
TARGET  	= nightingale.kernel
ISO			= nightingale.iso

CC 			= x86_64-elf-gcc
AS 			= nasm -felf64
LD 			= $(CC)

QEMU		= qemu-system-x86_64

CFLAGS  	= -Iinclude -ffreestanding -Wall -std=gnu11 -mno-red-zone -nostdlib -O0 -g -c
ASFLAGS 	= -g
LDFLAGS 	= -n -nostdlib -Tkernel/link.ld -z max-page-size=0x1000

SRCDIR		= kernel

CSOURCES	:= $(wildcard $(SRCDIR)/*.c)
ASOURCES	:= $(wildcard $(SRCDIR)/*.asm)
COBJECTS	:= $(CSOURCES:$(SRCDIR)/%.c=$(SRCDIR)/%.c.o)
AOBJECTS	:= $(ASOURCES:$(SRCDIR)/%.asm=$(SRCDIR)/%.asm.o)
OBJECTS		:= $(AOBJECTS) $(COBJECTS)

MEM			?= 64M

.PHONY: 	all clean iso run debug dump

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECTS) -lgcc

$(COBJECTS): $(SRCDIR)/%.c.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(AOBJECTS): $(SRCDIR)/%.asm.o : $(SRCDIR)/%.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(SRCDIR)/*.o
	rm -f $(TARGET)
	rm -f $(ISO)

$(ISO): $(TARGET)
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(TARGET) isodir/boot
	grub-mkrescue -o $(ISO) isodir/
	rm -rf isodir

iso: $(ISO)

run: $(ISO)
	$(QEMU) -cdrom $(ISO) -m $(MEM) -vga std -no-reboot -monitor stdio

debug: $(ISO)
	$(QEMU) -cdrom $(ISO) -m $(MEM) -vga std -no-reboot -monitor stdio -d cpu_reset -S -s

debugrst: $(ISO)
	$(QEMU) -cdrom $(ISO) -m $(MEM) -vga std -no-reboot -monitor stdio -d cpu_reset

debugint: $(ISO)
	$(QEMU) -cdrom $(ISO) -m $(MEM) -vga std -no-reboot -monitor stdio -d cpu_reset,int

dump: $(TARGET)
	x86_64-elf-objdump -Mintel -d $(TARGET) | less

