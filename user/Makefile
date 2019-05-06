# vim: noet ts=8 sw=8 sts=8

PROGRAMS	= init sh echo test uname what bf strace bomb rsh net busy \
		  cat udpnc polltest malloctest threads heapdbg false float \
		  forks top bg clone insmod auto_test fcat
PACKAGES	= lua
FILES		= text_file
MODULES		= test_mod.ko thread_mod.ko

INCLUDE		= -I../include -I. -I./include -I./libc

OPT		= -O0
DEBUG		= -g
WARNING		= -Wall -Wextra -Werror -Wpedantic -pedantic
EXTRA_CFLAGS	=
EXTRA_WARNING	= 

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###

SRCDIR		= .
BUILDDIR	= $(shell pwd)/../build$(ARCH)/user

export BUILDDIR

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


INITFS		= initfs

MAKEFILE	= Makefile

LIBC		= $(BUILDDIR)/libc.a
BINARIES	= $(addprefix $(BUILDDIR)/, $(PROGRAMS))
MD_MODULES	= $(addprefix $(BUILDDIR)/, $(MODULES))

.PHONY: all clean lua
.PRECIOUS: $(BUILDDIR)/%.c.o

all: $(INITFS)

include $(shell find $(BUILDDIR) -name "*.d" $(N))

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@
	@echo "HCC" $(notdir $<)

$(BUILDDIR)/%: $(BUILDDIR)/%.c.o $(LIBC)
	$(Q)$(CC) $(CFLAGS) $< -o $@ -L$(BUILDDIR) -lc -lgcc
	@#echo "HCC" $(notdir $@)

$(BUILDDIR)/%.ko: $(shell find modules)
	$(Q)make -C modules

$(LIBC): $(LIBCOBJ)
	$(Q)ar rcs $(LIBC) $(LIBCOBJ)
	@echo "AR" $(notdir $(LIBC))

$(INITFS): $(BINARIES) $(FILES) $(MD_MODULES)
	@cp -t $(BUILDDIR) $(FILES)
	$(Q)cd $(BUILDDIR); \
	  tar cf $(INITFS) $(PROGRAMS) $(FILES) $(MODULES)
	@echo "TAR initfs"

clean:
	rm -f $(INITFS)
	rm -f $(LIBC)

