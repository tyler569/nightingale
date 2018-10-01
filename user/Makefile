# vim: noet ts=8 sw=8 sts=8

CC		= i686-elf-gcc
AS		= nasm -felf32
LD		= $(CC)

INITFS		= initfs

PROGRAMS	= init sh echo test uname what bf strace bomb rsh net busy
FILES		= text_file

INCLUDE		= -I../include -I. -I./include -I./libc

OPT		= -O0
DEBUG		= -g
WARNING		= -Wall -Wextra -Werror -Wpedantic -pedantic
EXTRA_CFLAGS	=
EXTRA_WARNING	= 

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###

LIBCSRC		= $(shell find ./libc -name "[^_]*.c")
LIBCHDR		= $(shell find ./libc -name "[^_]*.h")

LIBCOBJ		= $(addprefix $(BUILDDIR)/, $(LIBCSRC:.c=.c.o))

# TODO: remove allowed warnings

CFLAGS		= $(INCLUDE) $(OPT) $(DEBUG) $(WARNING)			\
		  -std=c11 -nostdlib -ffreestanding			\
		  -mno-red-zone -fno-asynchronous-unwind-tables		\
		  -fno-strict-aliasing					\
		  -fno-omit-frame-pointer				\
		  -Wno-unused-variable					\
		  -Wno-unused-parameter					\
		  -Wno-sign-compare					\
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)


SRCDIR		= .
BUILDDIR	= ../build/user

MAKEFILE	= Makefile

LIBC		= libc/libc.a
BINARIES	= $(addprefix $(BUILDDIR)/, $(PROGRAMS))

.PHONY: all clean
.PRECIOUS: $(BUILDDIR)/%.c.o

all: $(INITFS)

include $(shell find $(BUILDDIR) -name "*.d")

$(BUILDDIR)/%.c.o: %.c %.h
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%: $(BUILDDIR)/%.c.o $(LIBC)
	$(CC) $(CFLAGS) $< -o $@ -L./libc -lc -lgcc

$(LIBC): $(LIBCOBJ)
	ar rcs $(LIBC) $(LIBCOBJ)

$(INITFS): $(BINARIES) $(FILES)
	cp -t $(BUILDDIR) $(FILES)
	cd $(BUILDDIR); \
	  tar cf $(INITFS) $(PROGRAMS) $(FILES)
	cp $(BUILDDIR)/$(INITFS) .

clean:
	rm -f $(INITFS)
	rm -f $(LIBC)

