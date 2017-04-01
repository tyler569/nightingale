
TARGET		= nightingale.kernel
ISO			= nightingale.iso

CC			= clang -target x86_64-unknown-none
AS			= nasm -felf64
LD			= ld.lld

MEM			?= 64M

QEMU		= qemu-system-x86_64
QEMUOPTS	= -m $(MEM) -vga std -no-reboot -monitor stdio

INCLUDE		= -Iinclude -Ilibc/include
OPT_LEVEL	?= 0
CFLAGS		= $(INCLUDE) -ffreestanding -Wall -std=c99 -mno-red-zone \
			  -nostdlib -O$(OPT_LVL) -g -c -mno-sse -mno-80387

ASFLAGS		= -g -F dwarf
LDFLAGS		= -nostdlib -Tkernel/link.ld -z max-page-size=0x1000

SRCDIR		= kernel

CSOURCES	:= $(shell find $(SRCDIR) -name "*.c")
ASOURCES	:= $(shell find $(SRCDIR) -name "*.asm")
COBJECTS	:= $(CSOURCES:.c=.c.o)
AOBJECTS	:= $(ASOURCES:.asm=.asm.o)
OBJECTS		:= $(AOBJECTS) $(COBJECTS)

.PHONY:		all release clean iso run debug dump

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

%.asm: 
	# stop it complaining about circular dependancy because %: %.o

%.asm.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
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
	$(QEMU) -cdrom $(ISO) $(QEMUOPTS)

runint: $(ISO)
	$(QEMU) -cdrom $(ISO) $(QEMUOPTS) -d cpu_reset,int

debug: $(ISO)
	$(QEMU) -cdrom $(ISO) $(QEMUOPTS) -d cpu_reset -S -s

debugint: $(ISO)
	$(QEMU) -cdrom $(ISO) $(QEMUOPTS) -d cpu_reset,int -S -s

dump: $(TARGET)
	# llvm-objdump -x86-asm-symtax=intel -disassemble $(TARGET)
	llvm-objdump -x86-asm-syntax=intel -disassemble $(TARGET) | less

