# vim: noet ts=8 sw=8 sts=8

# TODO: these are not used
#WARNING		= -Wall -Wextra -Werror -Wpedantic -pedantic
EXTRA_CFLAGS	=
EXTRA_WARNING	= 

SRCDIR		= .

INCLUDE		= -I$(INCDIR) -I$(INCDIR)/nc -I.

ifeq ($(NG), 1)
BUILDDIR	= $(shell pwd)/../build$(ARCH)/libknc
CFLAGS		= $(KCFLAGS) -D_NG=1
INCLUDE		+= -I$(KERNELDIR)/include/
LIBCSRC		= \
	ctype.c \
	string.c \
	stdio.c \
	malloc.c

CCSTR		= "XCC"

else
BUILDDIR	= $(shell pwd)/../build$(ARCH)/libnc
LIBCSRC		= $(shell find . -name "[^_]*.[cS]")

CCSTR		= "HCC"
endif

LIBCOBJ		= $(addprefix $(BUILDDIR)/, $(addsuffix .o,$(LIBCSRC)))

MAKEFILE	= Makefile

.PHONY: all clean
.PRECIOUS: $(BUILDDIR)/%.c.o

all: $(LIBC)

include $(shell find $(BUILDDIR) -name "*.d" $(N))

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(BUILDDIR)/$(dir $<)
	@echo $(CCSTR) $(notdir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(INCLUDE) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.S.o: %.S
	@mkdir -p $(BUILDDIR)/$(dir $<)
	@echo "HAS" $(notdir $<)
	$(Q)$(CC) -MD -MF $(BUILDDIR)/$<.d $(INCLUDE) $(CFLAGS) -c $< -o $@

$(LIBKNC): $(LIBCOBJ)
	@echo "AR" $(notdir $(LIBKNC))
	$(Q)ar rcs $(LIBKNC) $(LIBCOBJ)

$(LIBC): $(LIBCOBJ)
	@echo "AR" $(notdir $(LIBC))
	$(Q)ar rcs $(LIBC) $(LIBCOBJ)

clean:
	rm -f $(BUILDDIR)/*.o
	rm -f $(INITFS)
	rm -f $(LIBC)

