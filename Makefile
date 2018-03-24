# vim: set noet ts=8 sw=8 sts=8

CC			= x86_64-elf-gcc
AS			= nasm -felf64
LD			= $(CC)

VM			= qemu-system-x86_64
VMMEM		?= 64M
VMOPTS		= -vga std -no-reboot
VMOPTS		+= -m $(VMMEM)

INCLUDE		= -Iinclude -Ikernel -Ikernel/include

CFLAGS		?= -O0 -g

override CFLAGS	:= $(INCLUDE) $(CFLAGS) -Wall -std=c11 -flto     \
			-nostdlib -ffreestanding -mcmodel=kernel      		\
			-mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-3dnow   \
			-mno-red-zone -fno-asynchronous-unwind-tables       \
			-fstack-protector-strong -fno-omit-frame-pointer    \
			-D__is_ng_kernel

ifdef TESTING
	CFLAGS += -D__is_ng_test
endif

ifdef SMP
	# NOTE: this makes different things at compile vs run time
	# maybe I should export a script that runs the built ngk
	# or something like that
	VMOPTS += -smp 2
	CFLAGS += -D__is_smp
endif

override AFLAGS	:= $(AFLAGS)

LINKSCRIPT	= kernel/arch/x86/test_hh_link.ld
LDFLAGS		= -nostdlib -T$(LINKSCRIPT) -zmax-page-size=0x1000 -g
LBLIBS		= -lgcc

SRCDIR		= kernel
MAKEFILE	= Makefile

KERNEL		= ngk
ISO			= ngos.iso

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###
CSRC		= $(shell find $(SRCDIR) -name "[^_]*.c")
CHDR		= $(shell find $(SRCDIR) -name "[^_]*.h")
ASRC		= $(shell find $(SRCDIR) -name "[^_]*.asm")

COBJ		= $(CSRC:.c=.c.o)
AOBJ		= $(ASRC:.asm=.asm.o)

OBJECTS		= $(AOBJ) $(COBJ)


.PHONY: all clean iso run runscreen runmon runint debug debugint dump dumps dump32


all: $(ISO)

$(KERNEL): $(OBJECTS) $(MAKEFILE) $(LINKSCRIPT)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJECTS) $(LDLIBS)
	rm -f $(KERNEL)tmp*

%.asm: 
	# stop it complaining about circular dependancy because %: %.o

%.asm.o: %.asm $(MAKEFILE)
	$(AS) $(AFLAGS) $< -o $@

# gcc is now set to generate .d dependancy files
# they are included to add the dependancy information here
#
# Theoretically this means we will rebuild everything that
# depends on stuff I change.
#
# This is still tenative as to whether I will keep it
include $(shell find . -name "*.d")

%.c.o: %.c %.h $(MAKEFILE)
	$(CC) -MD -MF $<.d $(CFLAGS) -c $< -o $@

%.c.o: %.c $(MAKEFILE)
	$(CC) -MD -MF $<.d $(CFLAGS) -c $< -o $@

clean:
	rm -f $(shell find . -name "*.o")
	rm -f $(shell find . -name "*.d")
	rm -f $(KERNEL)
	rm -f $(ISO)

$(ISO): $(KERNEL) kernel/grub.cfg
	mkdir -p isodir/boot/grub
	cp kernel/grub.cfg isodir/boot/grub
	cp $(KERNEL) isodir/boot
	cp user/test_user_edited isodir/boot
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

