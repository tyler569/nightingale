# vim: noet ts=8 sw=8 sts=8

INCLUDE		= -I$(INCDIR) -I$(INCDIR)/nc

WARNING		= -Wall -Wextra -Werror -Wpedantic -pedantic

MD		= $(BUILDDIR)/user/sh_build
SH		= $(MD)/sh

CSRC		= $(wildcard *.c)
OBJECTS		= $(addprefix $(MD)/, $(CSRC:.c=.c.o))

MAKEFILE	= Makefile

.PHONY: all clean
.PRECIOUS: $(MD)/%.c.o

all: $(SH)

include $(shell find $(MD) -name "*.d" $(N))

$(LIBC):

%.c:

$(MD)/%.c.o: %.c
	@mkdir -p $(MD)/$(dir $<)
	@echo "HCC" $(notdir $<)
	$(Q)$(CC) -MD -MF $(MD)/$<.d $(INCLUDE) $(CFLAGS) -c $< -o $@

$(SH): $(OBJECTS) $(LIBC)
	@echo "HLD" $(notdir $@)
	$(Q)$(LD) $(LDFLAGS) $(OBJECTS) -o $@ -lc -lgcc -Wl,--gc-sections
	@#strip $@

clean:
	find $(MD) -name '*.o' | xargs rm -f

