# vim: noet ts=8 sw=8 sts=8

TARGETS		= test_mod.ko

MAKEFILE	= Makefile

MD_TARGETS	= $(addprefix $(BUILDDIR)/, $(TARGETS))

.PHONY: all clean

all: $(MD_TARGETS)

$(BUILDDIR)/%.ko: %.c $(MAKEFILE)
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CC) $(INCLUDE) $(KCFLAGS) -g0 -c $< -o $@
	@echo "XCC" $(notdir $<)

