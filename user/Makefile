# vim: noet ts=8 sw=8 sts=8

PROGRAMS	= init sh echo uname what bf strace bomb \
		  cat threads heapdbg false forks \
		  top clone insmod fcat cxx args
FILES		= text_file
PACKAGES	= lua
FILES		= text_file test.lua fib.lua
MODULES		= test_mod.ko thread_mod.ko cxx.ko

INCLUDE		= -I../include -I. -I./include -I./libc

WARNING		= -Wall -Wextra -Werror -Wpedantic -pedantic
EXTRA_CFLAGS	=
EXTRA_WARNING	= 

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###

SRCDIR		= .
BUILDDIR	= $(shell pwd)/../build$(ARCH)/user

export BUILDDIR

LIBCSRC		= $(shell find ./libc -name "[^_]*.[cS]")
LIBCHDR		= $(shell find ./libc -name "[^_]*.h")

LIBCOBJ		= $(addprefix $(BUILDDIR)/, $(addsuffix .o,$(LIBCSRC)))

# TODO: remove allowed warnings

CFLAGS		= $(INCLUDE) $(OPT) $(DEBUG) $(WARNING) \
		  -std=c11 -nostdlib -ffreestanding \
		  -fno-omit-frame-pointer \
		  -Wno-unused-variable \
		  -Wno-unused-parameter \
		  -Wno-sign-compare \
		  -ffunction-sections \
		  -fno-strict-aliasing \
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)

CXXFLAGS	= $(INCLUDE) $(OPT) $(DEBUG) $(WARNING) \
		  -xc++ -std=c++17 -nostdlib -ffreestanding \
		  -fno-omit-frame-pointer \
		  -Wno-unused-variable \
		  -Wno-unused-parameter \
		  -Wno-sign-compare \
		  -ffunction-sections \
		  -fno-strict-aliasing \
		  -fno-exceptions -fno-rtti \
		  $(EXTRA_CFLAGS) $(EXTRA_WARNING)
CXX		= $(CC)


INITFS		= initfs

MAKEFILE	= Makefile

LIBC		= $(BUILDDIR)/libc.a
LIBM		= $(BUILDDIR)/libm.a
BINARIES	= $(addprefix $(BUILDDIR)/, $(PROGRAMS))
MD_MODULES	= $(addprefix $(BUILDDIR)/, $(MODULES))
LUA		= $(BUILDDIR)/lua

ifdef BUILD_LUA
TAR_CONTENTS	= $(PROGRAMS) $(FILES) $(MODULES) libc.a lua
INITFS_DEPS	= $(BINARIES) $(FILES) $(MD_MODULES) $(LUA)
else
TAR_CONTENTS	= $(PROGRAMS) $(FILES) $(MODULES) libc.a
INITFS_DEPS	= $(BINARIES) $(FILES) $(MD_MODULES)
endif

export LIBC LIBM

.PHONY: all clean
.PRECIOUS: $(BUILDDIR)/%.c.o

all: $(INITFS)

include $(shell find $(BUILDDIR) -name "*.d" $(N))

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@
	@echo "HCC" $(notdir $<)

$(BUILDDIR)/%.cc.o: %.cc
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CXX) -MD -MF $(BUILDDIR)/$<.d $(CXXFLAGS) -c $< -o $@
	@echo "HXX" $(notdir $<)

$(BUILDDIR)/%.S.o: %.S
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@
	@echo "HAS" $(notdir $<)

$(BUILDDIR)/%: $(BUILDDIR)/%.c.o $(LIBC)
	$(Q)$(CC) $(CFLAGS) $< -o $@ -L$(BUILDDIR) -lc -lgcc -Wl,--gc-sections
	@#strip $@
	@#echo "HCC" $(notdir $@)

$(BUILDDIR)/%: $(BUILDDIR)/%.cc.o $(LIBC)
	$(Q)$(CC) $(CFLAGS) $< -o $@ -L$(BUILDDIR) -lc -lgcc -Wl,--gc-sections
	@#strip $@
	@#echo "HCC" $(notdir $@)

$(BUILDDIR)/%.ko: $(shell find modules)
	$(Q)make -C modules

$(LIBC): $(LIBCOBJ)
	$(Q)ar rcs $(LIBC) $(LIBCOBJ)
	@echo "AR" $(notdir $(LIBC))

$(LIBM): $(LIBC)
	make AS="$(CC)" -C libm

$(LUA): $(LIBC) $(LIBM) $(shell find lua)
	make -C lua
	cp ./lua/src/lua $(BUILDDIR)

$(INITFS): $(INITFS_DEPS)
	@cp -t $(BUILDDIR) $(FILES)
	$(Q)cd $(BUILDDIR); \
	  tar cf $(INITFS) $(TAR_CONTENTS)
	@echo "TAR initfs"

clean:
	rm -f $(INITFS)
	rm -f $(LIBC)
	make -C lua clean

