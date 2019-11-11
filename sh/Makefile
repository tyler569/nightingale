# vim: noet ts=8 sw=8 sts=8

INCDIR		= .
BUILDDIR	= .
CFLAGS		= -Og -g


#INCLUDE		= -I$(INCDIR) -I$(INCDIR)/nc
INCLUDE		= 

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
	echo $(OBJECTS)
	@echo "HLD" $(notdir $@)
	$(Q)$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	@#strip $@

clean:
	find $(MD) -name '*.o' | xargs rm -f

