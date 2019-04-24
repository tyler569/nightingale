# vim: noet ts=8 sw=8 sts=8

PROGRAMS	= init sh echo test uname what bf strace bomb rsh net busy \
		  cat udpnc polltest malloctest threads heapdbg false float \
		  forks top bg clone
PACKAGES	= lua
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

CFLAGS		= $(INCLUDE) $(OPT) $(DEBUG) $(WARNING) \
		  -std=c11 -nostdlib -ffreestanding \
		  -fno-omit-frame-pointer \
		  -Wno-unused-variable \
		  -Wno-unused-parameter \
		  -Wno-sign-compare \
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)


SRCDIR		= .
BUILDDIR	= ../build$(ARCH)/user
INITFS		= initfs

MAKEFILE	= Makefile

LIBC		= $(BUILDDIR)/libc.a
BINARIES	= $(addprefix $(BUILDDIR)/, $(PROGRAMS))

.PHONY: all clean lua
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
	$(CC) $(CFLAGS) $< -o $@ -L./$(BUILDDIR) -lc -lgcc

$(LIBC): $(LIBCOBJ)
	ar rcs $(LIBC) $(LIBCOBJ)

$(INITFS): $(BINARIES) $(FILES)
	cp -t $(BUILDDIR) $(FILES)
	cd $(BUILDDIR); \
	  tar cf $(INITFS) $(PROGRAMS) $(FILES)
	# cp $(BUILDDIR)/$(INITFS) .

clean:
	rm -f $(INITFS)
	rm -f $(LIBC)

