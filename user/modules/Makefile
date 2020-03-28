# vim: noet ts=8 sw=8 sts=8

TARGETS		= $(MODULES)

MD		= $(BUILDDIR)/user
MAKEFILE	= Makefile
MD_TARGETS	= $(addprefix $(MD)/, $(TARGETS))
INCLUDE		= -I$(INCDIR) -I$(INCDIR)/nc -I$(KERNELDIR)/include -I$(REPO)/linker/include

.PHONY: all clean

all: $(MD_TARGETS)

$(MD)/%.ko: %.c $(MAKEFILE)
	@mkdir -p $(MD)/$(dir $<)
	$(Q)$(CC) $(INCLUDE) $(KCFLAGS) -g0 -c $< -o $@
	@echo "XCC" $(notdir $<)

