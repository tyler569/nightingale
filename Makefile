
#
#
#
#
#
#

TARGET		= kernel
ISO			= nightingale.iso

CC 			= i686-elf-gcc -std=c99 -c
ASM 		= nasm -f elf32 # i686-elf-as
LINKER 		= i686-elf-gcc
RM			= rm -rf

CFLAGS 		= -Wall -I./include -ffreestanding
ASMFLAGS 	= 
LDFLAGS 	= -T etc/kernel-link.ld -nostdlib -lgcc

SRCDIR		= src
OBJDIR		= obj
BINDIR		= bin

CSOURCES	:= $(wildcard $(SRCDIR)/*.c)
ASOURCES	:= $(wildcard $(SRCDIR)/*.asm)
COBJECTS	:= $(CSOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
AOBJECTS	:= $(ASOURCES:$(SRCDIR)/%.asm=$(OBJDIR)/%.o)
OBJECTS		:= $(COBJECTS) $(AOBJECTS)


all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) -o $@ $(LDFLAGS) $(OBJECTS)

$(COBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(AOBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

.PHONY: iso
iso: $(ISO)

$(ISO): $(BINDIR)/$(TARGET)
	mkdir -p isodir/boot/grub
	cp etc/grub.cfg isodir/boot/grub
	cp bin/kernel isodir/boot
	grub-mkrescue isodir -o $@
	rm -rf isodir

.PHONY: cdrom
cdrom: iso
	qemu-system-i386 -curses -cdrom $(ISO)

.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) $(BINDIR)/$(TARGET)
	$(RM) $(ISO)

.PHONY: run
run: all
	qemu-system-i386 -curses -kernel $(BINDIR)/$(TARGET)



#all:
#	nasm -f elf32 kernel.asm -o kasm.o
#	nasm -f elf32 move_cursor.asm -o move_cursor.o
#	gcc -m32 -c kernel.c -o kc.o
#	gcc -m32 -c utils.c -o utils.o
#	ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o utils.o move_cursor.o
