
TARGET		= nightingale.kernel
ISO			= nightingale.iso

CC			= clang -target x86_64-unknown-none
CXX			= $(CC)
AS			= nasm -felf64
LD			= ld.lld

MEM			?= 64M

QEMU		= qemu-system-x86_64
QEMUOPTS	= -m $(MEM) -vga std -no-quit -no-reboot -monitor stdio

INCLUDE		= -Iinclude -Ikernel
OPT_LEVEL	?= 0

CFLAGS		= $(INCLUDE) -ffreestanding -Wall -std=c99 -mno-red-zone \
			  -nostdlib -O$(OPT_LVL) -g -c -mno-sse -mno-80387 \
			  -fno-asynchronous-unwind-tables -flto

ASFLAGS		= -g -F dwarf
LDFLAGS		= -nostdlib -Tkernel/link.ld -z max-page-size=0x1000

SRCDIR		= kernel

CSRC	:= $(shell find $(SRCDIR) -name "*.c")
ASMSRC	:= $(shell find $(SRCDIR) -name "*.asm")

COBJ	:= $(CSRC:.c=.c.o)
ASMOBJ	:= $(ASMSRC:.asm=.asm.o)

OBJECTS		:= $(ASMOBJ) $(COBJ)

.PHONY:		all release clean iso run debug dump

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECTS)
	rm -f $(TARGET)tmp*

%.asm: 
	# stop it complaining about circular dependancy because %: %.o

%.asm.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.cpp.o: %.cpp
	$(CC) $(CXXFLAGS) $< -o $@

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
	# llvm-objdump -x86-asm-symtax=intel -disassemble $(TARGET) | less
	objdump -Mintel -d $(TARGET) | less

dump32: $(TARGET)
	objdump -Mintel,i386 -d $(TARGET) | less
