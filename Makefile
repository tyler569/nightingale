# vim: set noet ts=4 sw=4 sts=4

CFLAGS		?= -O0 -ggdb

CC			= x86_64-elf-gcc
AS			= nasm -felf64
LD			= $(CC)

VM			= qemu-system-x86_64
VMMEM		?= 64M
VMOPTS		= -vga std -no-reboot -smp 2 -display none
VMOPTS		+= -m $(VMMEM)

INCLUDE		= -Iinclude -Ikernel -Ikernel/include

CFLAGS		:= $(CFLAGS) $(INCLUDE) -Wall -std=c11 					\
			-nostdlib -nostdinc -ffreestanding -mcmodel=kernel		\
			-mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-3dnow		\
			-mno-red-zone -fno-asynchronous-unwind-tables			\
			-fstack-protector -fno-omit-frame-pointer

AFLAGS		=

LINKSCRIPT	= kernel/arch/x86/link.ld
LDFLAGS		= -nostdlib -T$(LINKSCRIPT) -zmax-page-size=0x1000
LBLIBS		= -lgcc

SRCDIR		= kernel
MAKEFILE	= Makefile

TARGET		= nightingale.kernel
ISO			= nightingale.iso

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###
CSRC		= $(shell find $(SRCDIR) -name "[^_]*.c")
CHDR		= $(shell find $(SRCDIR) -name "[^_]*.h")
ASRC		= $(shell find $(SRCDIR) -name "[^_]*.asm")

COBJ		= $(CSRC:.c=.c.o)
AOBJ		= $(ASRC:.asm=.asm.o)

OBJECTS		= $(AOBJ) $(COBJ)


.PHONY: all clean iso run runserial runint debug debugint dump dumps dump32


all: $(TARGET)

$(TARGET): $(OBJECTS) $(MAKEFILE) $(LINKSCRIPT)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LDLIBS)
	rm -f $(TARGET)tmp*

%.asm: 
	# stop it complaining about circular dependancy because %: %.o

%.asm.o: %.asm
	$(AS) $(AFLAGS) $< -o $@

# gcc is now set to generate .d dependancy files
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
	$(VM) -cdrom $(ISO) $(VMOPTS) -serial stdio

runserial: $(ISO)
	$(VM) -cdrom $(ISO) $(VMOPTS) -monitor stdio

runint: $(ISO)
	$(VM) -cdrom $(ISO) $(VMOPTS) -monitor stdio -d cpu_reset,int

debug: $(ISO)
	$(VM) -cdrom $(ISO) $(VMOPTS) -serial stdio -S -s

debugint: $(ISO)
	$(VM) -cdrom $(ISO) $(VMOPTS) -d cpu_reset,int -S -s

dump: $(TARGET)
	x86_64-elf-objdump -Mintel -d $(TARGET) | less

dumps: $(TARGET)
	x86_64-elf-objdump -Mintel -dS $(TARGET) | less

dump32: $(TARGET)
	x86_64-elf-objdump -Mintel,i386 -d $(TARGET) | less

