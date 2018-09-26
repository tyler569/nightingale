# vim: noet ts=8 sw=8 sts=8

#CC		= x86_64-elf-gcc
#AS		= nasm -felf64
#CC		= i686-elf-gcc
#AS		= nasm -felf32
#LD		= $(CC)

KERNEL		= ngk

INCLUDE		= -I../include -I. -I./include

EXTRA_CFLAGS	=
OPT		= -O0
DEBUG		= -g

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###

ifeq ($(ARCH), X86_64)
CSRC		= $(shell find $(SRCDIR) -name "[^_]*.c" | grep -v "32")
CHDR		= $(shell find $(SRCDIR) -name "[^_]*.h" | grep -v "32")
ASRC		= $(shell find $(SRCDIR) -name "[^_]*.asm" | grep -v "32")

ARCH_CFLAGS	= -mcmodel=kernel
LINKSCRIPT	= $(SRCDIR)/arch/x86/link_hh64.ld

else ifeq ($(ARCH), I686)
CSRC		= $(shell find $(SRCDIR) -name "[^_]*.c" | grep -v "64")
CHDR		= $(shell find $(SRCDIR) -name "[^_]*.h" | grep -v "64")
ASRC		= $(shell find $(SRCDIR) -name "[^_]*.asm" | grep -v "64")

ARCH_CFLAGS	=
LINKSCRIPT	= $(SRCDIR)/arch/x86/link_hh32.ld

endif

# TODO: remove allowed warnings

CFLAGS		= $(INCLUDE) $(EXTRA_CFLAGS) $(OPT) $(DEBUG)		\
		  $(ARCH_CFLAGS)					\
		  -std=c11 -flto -nostdlib -ffreestanding		\
		  -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-3dnow	\
		  -mno-red-zone -fno-asynchronous-unwind-tables		\
		  -fno-strict-aliasing					\
		  -fno-omit-frame-pointer				\
		  -DNIGHTINGALE_VERSION="\"`git describe --tags`\""	\
		  -Wall -Wextra -Werror -Wpedantic -pedantic		\
		  -Wno-unused-variable					\
		  -Wno-unused-parameter					\
		  -Wno-sign-compare					\
		  -fsanitize=undefined


ifdef TESTING
	CFLAGS += -D__is_ng_test
endif

override AFLAGS	:= $(AFLAGS)

SRCDIR		= .
BUILDDIR	= ../build

MAKEFILE	= Makefile

LDFLAGS		= -nostdlib -T$(LINKSCRIPT) -zmax-page-size=0x1000 -g
LBLIBS		= -lgcc

COBJ		= $(addprefix $(BUILDDIR)/, $(CSRC:.c=.c.o))
AOBJ		= $(addprefix $(BUILDDIR)/, $(ASRC:.asm=.asm.o))

OBJECTS		= $(AOBJ) $(COBJ)

.PHONY: all clean

all: $(KERNEL)

$(KERNEL): $(OBJECTS) $(MAKEFILE) $(LINKSCRIPT)
	$(LD) $(LDFLAGS) -o $(KERNEL) $(OBJECTS) $(LDLIBS)
	rm -f $(KERNEL)tmp*

%.asm: 
	# stop it complaining about circular dependancy because %: %.o

$(BUILDDIR)/%.asm.o: %.asm $(MAKEFILE)
	mkdir -p $(BUILDDIR)/$(dir $<)
	$(AS) $(AFLAGS) $< -o $@

# gcc is now set to generate .d dependancy files
# they are included to add the dependancy information here
#
# Theoretically this means we will rebuild everything that
# depends on stuff I change.
#
# This is still tenative as to whether I will keep it
include $(shell find . -name "*.d")

$(BUILDDIR)/%.c.o: %.c %.h $(MAKEFILE)
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.c.o: %.c $(MAKEFILE)
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@

clean:
	rm -f $(shell find . -name "*.o")
	rm -f $(shell find . -name "*.d")
	rm -f $(KERNEL)

