
CC          = clang-5.0 -target x86_64-unknown-none
AS          = nasm -felf64
LD          = ld.lld

VM          = qemu-system-x86_64
VMMEM       ?= 64M
VMOPTS      = -vga std -no-quit -no-reboot -monitor stdio
VMOPTS      = -vga std -no-quit -no-reboot -serial stdio
VMOPTS      += -m $(VMMEM)

INCLUDE     = -Iinclude -Ikernel

CFLAGS      = $(INCLUDE) -Wall -std=c11                             \
              -nostdlib -nostdinc -ffreestanding                    \
              -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone  \
              -fno-asynchronous-unwind-tables -mcmodel=large

ASFLAGS     =

ifdef RELEASE
CFLAGS		+= -O3
else
CFLAGS      += -g -O0
ASFLAGS     += -g -Fdwarf
endif

LINKSCRIPT  = kernel/link.ld
LDFLAGS     = -nostdlib -T$(LINKSCRIPT) -z max-page-size=0x1000

SRCDIR      = kernel
MAKEFILE    = Makefile

TARGET      = nightingale.kernel
LIBK		= nightingale-libk.a
ISO         = nightingale.iso

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###
CSRC        = $(shell find $(SRCDIR) -name "[^_]*.c")
ASMSRC      = $(shell find $(SRCDIR) -name "[^_]*.asm")

COBJ        = $(CSRC:.c=.c.o)
ASMOBJ      = $(ASMSRC:.asm=.asm.o)

OBJECTS     = $(ASMOBJ) $(COBJ)

.PHONY:     all release clean iso run debug dump allinone

all: $(TARGET)

$(TARGET): $(OBJECTS) $(MAKEFILE) $(LIBK)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBK)
	rm -f $(TARGET)tmp*

$(LIBK): libk/libk.c
	$(CC) $(CFLAGS) -c libk/libk.c -o $@.o
	ar rcs $@ $@.o

# Why does this not respect use-ld?
# It seems to be becasue I am cross compiling, it invokes gcc with use-ld=lld
# which then fails because it tries to pass -mno-x87 to gcc, which is invalid
allinone: $(ASMOBJ)
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
	rm -f $(LIBK)
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
	$(VM) -cdrom $(ISO) $(VMOPTS)

runint: $(ISO)
	$(VM) -cdrom $(ISO) $(VMOPTS) -d cpu_reset,int

debug: $(ISO)
	$(VM) -cdrom $(ISO) $(VMOPTS) -d cpu_reset -S -s

debugint: $(ISO)
	$(VM) -cdrom $(ISO) $(VMOPTS) -d cpu_reset,int -S -s

dump: $(TARGET)
	# llvm-objdump -x86-asm-symtax=intel -disassemble $(TARGET) | less
	objdump -Mintel -d $(TARGET) | less

dump32: $(TARGET)
	objdump -Mintel,i386 -d $(TARGET) | less

