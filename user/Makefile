# vim: noet ts=8 sw=8 sts=8

PROGRAMS	= init sh echo uname what bf strace bomb \
		  cat threads heapdbg false forks \
		  top clone insmod fcat cxx args pipe \
		  sg kill write segv echoserv sleep \
		  bg fio float heapdbg malloctest net rsh udpnc
FILES		= text_file
PACKAGES	= lua
FILES		= text_file test.lua fib.lua
MODULES		= test_mod.ko thread_mod.ko cxx.ko thcxx.ko

INCLUDE		= -I$(INCDIR) -I$(INCDIR)/nc
INCLUDEXX	= -I$(INCDIR) -I$(INCDIR)/nc -I$(INCDIR)/nx

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
LUA		= $(BUILDDIR)/lua_build/lua

ifdef BUILD_LUA
TAR_CONTENTS	= $(PROGRAMS) $(FILES) $(MODULES) lua
INITFS_DEPS	= $(BINARIES) $(FILES) $(MD_MODULES) $(LUA)
else
TAR_CONTENTS	= $(PROGRAMS) $(FILES) $(MODULES)
INITFS_DEPS	= $(BINARIES) $(FILES) $(MD_MODULES)
endif

LIBM		= $(LIBDIR)/libm.a

.PHONY: all clean
.PRECIOUS: $(BUILDDIR)/%.c.o $(BUILDDIR)/%.cc.o

all: $(INITFS)

include $(shell find $(BUILDDIR) -name "*.d" $(N))

$(LIBC):
$(LIBNX):

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(BUILDDIR)/$(dir $<)
	@echo "HCC" $(notdir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(INCLUDE) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.cc.o: %.cc
	@mkdir -p $(BUILDDIR)/$(dir $<)
	@echo "HCXX" $(notdir $<)
	$(Q)$(CXX) -MD -MF $(BUILDDIR)/$<.d $(INCLUDEXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.S.o: %.S
	@mkdir -p $(BUILDDIR)/$(dir $<)
	@echo "HAS" $(notdir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%: $(BUILDDIR)/%.c.o $(LIBC)
	@echo "HLD" $(notdir $@)
	$(Q)$(CC) $(LDFLAGS) $< -o $@ -lc -lgcc -Wl,--gc-sections
	@#strip $@

$(BUILDDIR)/%: $(BUILDDIR)/%.cc.o $(LIBC) $(LIBNX)
	@echo "HLD" $(notdir $@)
	$(Q)$(CC) $(LDFLAGS) $< -o $@ -lnx -lc -lgcc -Wl,--gc-sections
	@#strip $@

$(BUILDDIR)/%.ko: $(shell find modules)
	$(Q)make -C modules

$(LIBM): $(LIBC)
	make AS="$(CC)" -C libm

$(LUA): $(LIBM) $(shell find lua)
	make -C lua
	@cp -t $(BUILDDIR) $(BUILDDIR)/lua_build/lua

$(INITFS): $(INITFS_DEPS)
	@cp -t $(BUILDDIR) $(FILES)
	$(Q)cd $(BUILDDIR); \
	  tar cf $(INITFS) $(TAR_CONTENTS)
	$(Q)cp $(BUILDDIR)/initfs $(BUILDDIR)/../initfs
	@echo "TAR initfs"

clean:
	rm -f $(INITFS)
	find $(BUILDDIR) -name '*.o' | xargs rm -f
	make -C libm clean
	make -C lua clean

