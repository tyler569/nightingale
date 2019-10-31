# vim: noet ts=8 sw=8 sts=8

# TODO: these are not used
#WARNING		= -Wall -Wextra -Werror -Wpedantic -pedantic
EXTRA_CFLAGS	=
EXTRA_WARNING	= 

SRCDIR		= .

INCLUDE		= -I../include -I../include/nc -I.

ifeq ($(NG), 1)
BUILDDIR	= $(shell pwd)/../build$(ARCH)/libknc
CFLAGS		= $(KCFLAGS) -D_NG=1
INCLUDE		+= -I../include/ng
LIBCSRC		= \
	ctype.c \
	string.c

else
BUILDDIR	= $(shell pwd)/../build$(ARCH)/libnc

###
# SPECIFICALLY FORBIDS ANY FILES STARTING IN '_'
###
LIBCSRC		= $(shell find . -name "[^_]*.[cS]")
endif

LIBCOBJ		= $(addprefix $(BUILDDIR)/, $(addsuffix .o,$(LIBCSRC)))

MAKEFILE	= Makefile

.PHONY: all clean
.PRECIOUS: $(BUILDDIR)/%.c.o

all: $(LIBC)

include $(shell find $(BUILDDIR) -name "*.d" $(N))

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(INCLUDE) $(CFLAGS) -c $< -o $@
	@echo "HCC" $(notdir $<)

$(BUILDDIR)/%.S.o: %.S
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(INCLUDE) $(CFLAGS) -c $< -o $@
	@echo "HAS" $(notdir $<)

$(LIBKNC): $(LIBCOBJ)
	$(Q)ar rcs $(LIBKNC) $(LIBCOBJ)
	@echo "AR" $(notdir $(LIBKNC))

$(LIBC): $(LIBCOBJ)
	$(Q)ar rcs $(LIBC) $(LIBCOBJ)
	@echo "AR" $(notdir $(LIBC))

clean:
	rm -f $(BUILDDIR)/*.o
	rm -f $(INITFS)
	rm -f $(LIBC)

