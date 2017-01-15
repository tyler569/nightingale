
#
#
#
#
#
#

TARGET      = kernel

CC 			= gcc -std=c99 -c -m32
ASM 		= nasm -f elf32
LINKER 		= ld -o
RM			= rm -rf

CFLAGS 		= -Wall -I./include -nostdinc -fno-builtin
ASMFLAGS 	= 
LDFLAGS 	= -m elf_i386 -T link.ld

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
	@$(LINKER) $@ $(LDFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(COBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(AOBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.asm
	@$(ASM) $(ASMFLAGS) $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(RM) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(RM) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

.PHONY: run
run: all
	qemu-system-i386 -curses -kernel $(BINDIR)/$(TARGET)



#all:
#	nasm -f elf32 kernel.asm -o kasm.o
#	nasm -f elf32 move_cursor.asm -o move_cursor.o
#	gcc -m32 -c kernel.c -o kc.o
#	gcc -m32 -c utils.c -o utils.o
#	ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o utils.o move_cursor.o
