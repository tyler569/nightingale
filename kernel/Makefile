# vim: noet ts=8 sw=8 sts=8

#CC		= x86_64-elf-gcc
#AS		= nasm -felf64
#CC		= i686-elf-gcc
#AS		= nasm -felf32
#LD		= $(CC)

KERNEL		= ngk

INCLUDE		= -I../include -I. -I./include

OPT		= -O0
DEBUG		= -g
WARNING		= -Wall -Wextra -Werror -Wpedantic -pedantic
EXTRA_CFLAGS	=
EXTRA_WARNING	= 

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###

ifeq ($(ARCH), X86_64)
CSRC		= $(shell find $(SRCDIR) -name "[^_]*.c" | grep -v "32")
CHDR		= $(shell find $(SRCDIR) -name "[^_]*.h" | grep -v "32")
ASRC		= $(shell find $(SRCDIR) -name "[^_]*.asm" | grep -v "32")

ARCH_CFLAGS	= -mcmodel=kernel
LINKSCRIPT	= $(SRCDIR)/arch/x86/64/link_hh.ld

else ifeq ($(ARCH), I686)
CSRC		= $(shell find $(SRCDIR) -name "[^_]*.c" | grep -v "64")
CHDR		= $(shell find $(SRCDIR) -name "[^_]*.h" | grep -v "64")
ASRC		= $(shell find $(SRCDIR) -name "[^_]*.asm" | grep -v "64")

ARCH_CFLAGS	=
LINKSCRIPT	= $(SRCDIR)/arch/x86/32/link_hh.ld

endif

# TODO: remove allowed warnings

CFLAGS		= $(INCLUDE) $(OPT) $(DEBUG)				\
		  $(ARCH_CFLAGS) $(WARNING)				\
		  -std=c11 -nostdlib -ffreestanding			\
		  -mgeneral-regs-only					\
		  -mno-red-zone -fno-asynchronous-unwind-tables		\
		  -fno-strict-aliasing					\
		  -fno-omit-frame-pointer				\
		  -DNIGHTINGALE_VERSION="\"`git describe --tags`\""	\
		  -Wno-unused-variable					\
		  -Wno-unused-parameter					\
		  -Wno-sign-compare					\
		  -Wno-unused-function					\
		  -fsanitize=undefined					\
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)


ifdef TESTING
	CFLAGS += -D__is_ng_test
endif

override AFLAGS	:= $(AFLAGS)

SRCDIR		= .
BUILDDIR	= ../build

MAKEFILE	= Makefile

LDFLAGS		= -nostdlib -T$(LINKSCRIPT) -zmax-page-size=0x1000 -g

# future direction? :
# ifeq ($(findstring gcc,$(LD)), gcc)
# LDFLAGS		= -nostdlib -T$(LINKSCRIPT) -zmax-page-size=0x1000 -g
# else ifeq ($(findstring clang,$(LD)), clang)
# LDFLAGS		= -nostdlib -T$(LINKSCRIPT) -g
# endif

COBJ		= $(addprefix $(BUILDDIR)/, $(CSRC:.c=.c.o))
AOBJ		= $(addprefix $(BUILDDIR)/, $(ASRC:.asm=.asm.o))

OBJECTS		= $(AOBJ) $(COBJ)

.PHONY: all clean

all: $(KERNEL)

include $(shell find $(BUILDDIR) -name "*.d")

$(KERNEL): $(OBJECTS) $(MAKEFILE) $(LINKSCRIPT)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJECTS) -lgcc
	rm -f $(KERNEL)tmp*

%.asm: 
	# stop it complaining about circular dependancy because %: %.o

$(BUILDDIR)/%.asm.o: %.asm $(MAKEFILE)
	mkdir -p $(BUILDDIR)/$(dir $<)
	$(AS) $(AFLAGS) $< -o $@

$(BUILDDIR)/%.c.o: %.c %.h $(MAKEFILE)
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.c.o: %.c $(MAKEFILE)
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@

clean:
	rm -f $(KERNEL)

