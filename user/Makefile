# vim: noet ts=8 sw=8 sts=8

PROGRAMS	= init echo uname what bf strace bomb cat threads heapdbg \
		  false forks top clone insmod fcat cxx args pipe sg kill \
		  write segv echoserv sleep bg fio float heapdbg malloctest \
		  net rsh udpnc ls rot13 time multiread create crash
PACKAGES	= lua sh
FILES		= text_file test.lua fib.lua
MODULES		= test_mod.ko thread_mod.ko cxx.ko thcxx.ko

export INCLUDE	= -I$(INCDIR) -I$(INCDIR)/nc
INCLUDEXX	= -I$(INCDIR) -I$(INCDIR)/nc -I$(INCDIR)/nx

WARNING		= -Wall -Wextra -Werror -Wpedantic -pedantic
EXTRA_CFLAGS	=
EXTRA_WARNING	= 

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###

SRCDIR		= .
MD		= $(BUILDDIR)/user

INITFS		= initfs

MAKEFILE	= Makefile

BINARIES	= $(addprefix $(MD)/, $(PROGRAMS))
MD_MODULES	= $(addprefix $(MD)/, $(MODULES))
LUA		= $(MD)/lua_build/lua
SH		= $(MD)/sh_build/sh

ifdef BUILD_LUA
TAR_CONTENTS	= $(PROGRAMS) $(FILES) $(MODULES) lua sh
INITFS_DEPS	= $(BINARIES) $(FILES) $(MD_MODULES) $(LUA) $(SH)
else
TAR_CONTENTS	= $(PROGRAMS) $(FILES) $(MODULES) sh
INITFS_DEPS	= $(BINARIES) $(FILES) $(MD_MODULES) $(SH)
endif

LIBM		= $(LIBDIR)/libm.a

.PHONY: all clean
.PRECIOUS: $(MD)/%.c.o $(MD)/%.cc.o

all: $(INITFS)

include $(shell find $(MD) -name "*.d" $(N))

$(LIBC):
$(LIBNX):

$(MD)/%.c.o: %.c
	@mkdir -p $(MD)/$(dir $<)
	@echo "HCC" $(notdir $<)
	$(Q)$(CC) -MD -MF $(MD)/$<.d $(INCLUDE) $(CFLAGS) -c $< -o $@

$(MD)/%.cc.o: %.cc
	@mkdir -p $(MD)/$(dir $<)
	@echo "HCXX" $(notdir $<)
	$(Q)$(CXX) -MD -MF $(MD)/$<.d $(INCLUDEXX) $(CXXFLAGS) -c $< -o $@

$(MD)/%.S.o: %.S
	@mkdir -p $(MD)/$(dir $<)
	@echo "HAS" $(notdir $<)
	$(Q)$(CC) -MD -MF $(MD)/$<.d $(CFLAGS) -c $< -o $@

$(MD)/%: $(MD)/%.c.o $(LIBC)
	@echo "HLD" $(notdir $@)
	$(Q)$(LD) $(LDFLAGS) $< -o $@ -lc -lgcc -Wl,--gc-sections
	@#strip $@

$(MD)/%: $(MD)/%.cc.o $(LIBC) $(LIBNX)
	@echo "HLD" $(notdir $@)
	$(Q)$(LD) $(LDFLAGS) $< -o $@ -lnx -lc -lgcc -Wl,--gc-sections
	@#strip $@

$(MD)/%.ko: $(shell find modules)
	$(Q)$(MAKE) -C modules

$(LIBM): $(LIBC)
	make AS="$(CC)" -C libm

$(LUA): $(LIBM) $(shell find lua)
	$(MAKE) -C lua
	@cp -t $(MD) $(MD)/lua_build/lua

$(SH): $(LIBC) $(wildcard sh/*.c)
	echo $(shell pwd)
	$(MAKE) -C sh
	@cp -t $(MD) $(MD)/sh_build/sh # shouldn't be needed

$(INITFS): $(INITFS_DEPS)
	@cp -t $(MD) $(FILES)
	$(Q)cd $(MD); \
	  tar cf $(INITFS) $(TAR_CONTENTS)
	$(Q)cp $(MD)/initfs $(MD)/../initfs
	@echo "TAR initfs"

clean:
	rm -f $(INITFS)
	find $(MD) -name '*.o' | xargs rm -f
	$(MAKE) -C libm clean
	$(MAKE) -C lua clean
	$(MAKE) -C sh clean

