# vim: noet ts=8 sw=8 sts=8

# TODO: these are not used
#WARNING		= -Wall -Wextra -Werror -Wpedantic -pedantic
EXTRA_CFLAGS	=
EXTRA_WARNING	= 

SRCDIR		= .

INCLUDE		= -I$(INCDIR) -I$(INCDIR)/nc -I.

ifeq ($(NG), 1)
MD		= $(BUILDDIR)/libknc
CFLAGS		= $(KCFLAGS) -D_NG=1
INCLUDE		+= -I$(KERNELDIR)/include/
LIBCSRC		= \
	ctype.c \
	string.c \
	stdio.c \
	malloc.c \
	errno.c

CCSTR		= "XCC"

else
MD		= $(BUILDDIR)/libnc
LIBCSRC		= $(shell find . -name "[^_]*.[cS]")

CCSTR		= "HCC"
endif

LIBCOBJ		= $(addprefix $(MD)/, $(addsuffix .o,$(LIBCSRC)))

MAKEFILE	= Makefile

.PHONY: all clean
.PRECIOUS: $(MD)/%.c.o

all: $(LIBC)

include $(shell find $(MD) -name "*.d" $(N))

$(MD)/%.c.o: %.c
	@mkdir -p $(MD)/$(dir $<)
	@echo $(CCSTR) $(notdir $<)
	$(Q)$(CC) -MD -MF $(MD)/$<.d $(INCLUDE) $(CFLAGS) -c $< -o $@

$(MD)/%.S.o: %.S
	@mkdir -p $(MD)/$(dir $<)
	@echo "HAS" $(notdir $<)
	$(Q)$(CC) -MD -MF $(MD)/$<.d $(INCLUDE) $(CFLAGS) -c $< -o $@

$(LIBKNC): $(LIBCOBJ)
	@echo "AR" $(notdir $(LIBKNC))
	$(Q)ar rcs $(LIBKNC) $(LIBCOBJ)

$(LIBC): $(LIBCOBJ)
	@echo "AR" $(notdir $(LIBC))
	$(Q)ar rcs $(LIBC) $(LIBCOBJ)

clean:
	rm -f $(MD)/*.o
	rm -f $(INITFS)
	rm -f $(LIBC)

