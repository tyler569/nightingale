
TARGET		= nightingale.kernel
ISO			= nightingale.iso

CC			= clang-5.0 -target x86_64-unknown-none
AS			= nasm -felf64
LD			= ld.lld

MEM			?= 64M

QEMU		= qemu-system-x86_64
QEMUOPTS	= -m $(MEM) -vga std -no-quit -no-reboot -monitor stdio

INCLUDE		= -Iinclude -Ikernel
OPT_LEVEL	?= 0

CFLAGS		= $(INCLUDE) -ffreestanding -Wall -std=c11 -mno-red-zone 	\
			  -nostdlib -nostdinc -O$(OPT_LEVEL) -g -mno-sse -mno-80387	\
			  -fno-asynchronous-unwind-tables -mcmodel=large

ASFLAGS		= -g -F dwarf
LINKSCRIPT	= kernel/link.ld
LDFLAGS		= -nostdlib -T$(LINKSCRIPT) -z max-page-size=0x1000

SRCDIR		= kernel
MAKEFILE	= Makefile

CSRC	:= $(shell find $(SRCDIR) -name "*.c")
ASMSRC	:= $(shell find $(SRCDIR) -name "*.asm")

COBJ	:= $(CSRC:.c=.c.o)
ASMOBJ	:= $(ASMSRC:.asm=.asm.o)

OBJECTS		:= $(ASMOBJ) $(COBJ)

.PHONY:		all release clean iso run debug dump allinone

all: $(TARGET)

$(TARGET): $(OBJECTS) $(MAKEFILE)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECTS)
	rm -f $(TARGET)tmp*

allinone: $(ASMOBJ)
	# Why does this not respect use-ld?
	# I am trying to reproduce this behavior in linker_test
	# It seems to be becasue I am cross compiling, it invokes gcc with use-ld=lld
	# which then fails because it tries to pass -mno-x87 to gcc, which is invalid
	$(CC) -fuse-ld=lld $(CFLAGS) -T(LINKSCRIPT) -z max-page-size=0x1000 $(CSRC)

%.asm: 
	# stop it complaining about circular dependancy because %: %.o

%.asm.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# clang is now set to generate .d dependancy files
# they are included to add the dependancy information here
#
# Theoretically this means we will rebuild everything that
# depends on stuff I change.
#
# This is still tenative as to whether I will keep it
include $(shell find . -name "*.d")

%.c.o: %.c %.h
	$(CC) -MD -MF $<.d $(CFLAGS) -c $< -o $@
%.c.o: %.c
	$(CC) -MD -MF $<.d $(CFLAGS) -c $< -o $@

clean:
	rm -f $(shell find . -name "*.o")
	rm -f $(shell find . -name "*.d")
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

