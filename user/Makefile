# vim: noet ts=8 sw=8 sts=8

PROGRAMS	= init sh echo uname what bf strace bomb \
		  cat threads heapdbg false forks \
		  top clone insmod fcat cxx args
FILES		= text_file
PACKAGES	= lua
FILES		= text_file test.lua fib.lua
MODULES		= test_mod.ko thread_mod.ko cxx.ko

INCLUDE		= -I$(INCDIR) -I$(INCDIR)/nc

WARNING		= -Wall -Wextra -Werror -Wpedantic -pedantic
EXTRA_CFLAGS	=
EXTRA_WARNING	= 

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###

SRCDIR		= .
BUILDDIR	= $(shell pwd)/../build$(ARCH)/user

export BUILDDIR

INITFS		= initfs

MAKEFILE	= Makefile

BINARIES	= $(addprefix $(BUILDDIR)/, $(PROGRAMS))
MD_MODULES	= $(addprefix $(BUILDDIR)/, $(MODULES))
LUA		= $(BUILDDIR)/lua

ifdef BUILD_LUA
TAR_CONTENTS	= $(PROGRAMS) $(FILES) $(MODULES) lua
INITFS_DEPS	= $(BINARIES) $(FILES) $(MD_MODULES) $(LUA)
else
TAR_CONTENTS	= $(PROGRAMS) $(FILES) $(MODULES)
INITFS_DEPS	= $(BINARIES) $(FILES) $(MD_MODULES)
endif

.PHONY: all clean
.PRECIOUS: $(BUILDDIR)/%.c.o

all: $(INITFS)

include $(shell find $(BUILDDIR) -name "*.d" $(N))

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(INCLUDE) $(CFLAGS) -c $< -o $@
	@echo "HCC" $(notdir $<)

$(BUILDDIR)/%.cc.o: %.cc
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CXX) -MD -MF $(BUILDDIR)/$<.d $(INCLUDE) $(CXXFLAGS) -c $< -o $@
	@echo "HXX" $(notdir $<)

$(BUILDDIR)/%.S.o: %.S
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@
	@echo "HAS" $(notdir $<)

$(BUILDDIR)/%: $(BUILDDIR)/%.c.o
	$(Q)$(CC) $(LDFLAGS) $< -o $@ -lc -lgcc -Wl,--gc-sections
	@#strip $@
	@#echo "HCC" $(notdir $@)

$(BUILDDIR)/%: $(BUILDDIR)/%.cc.o
	$(Q)$(CC) $(LDFLAGS) $< -o $@ -lc -lgcc -Wl,--gc-sections
	@#strip $@
	@#echo "HCC" $(notdir $@)

$(BUILDDIR)/%.ko: $(shell find modules)
	$(Q)make -C modules

$(LIBM): $(LIBC)
	make AS="$(CC)" -C libm

$(LUA): $(LIBM) $(shell find lua)
	make -C lua
	cp ./lua/src/lua $(BUILDDIR)

$(INITFS): $(INITFS_DEPS)
	@cp -t $(BUILDDIR) $(FILES)
	$(Q)cd $(BUILDDIR); \
	  tar cf $(INITFS) $(TAR_CONTENTS)
	$(Q)cp $(BUILDDIR)/initfs $(BUILDDIR)/../initfs
	@echo "TAR initfs"

clean:
	rm -f $(INITFS)
	find $(BUILDDIR) -name '*.o' | xargs rm
	make -C libm clean
	make -C lua clean

