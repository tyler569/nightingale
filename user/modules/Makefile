# vim: noet ts=8 sw=8 sts=8

TARGETS		= test_mod.ko thread_mod.ko cxx.ko

MAKEFILE	= Makefile

MD_TARGETS	= $(addprefix $(BUILDDIR)/, $(TARGETS))

INCLUDE		= -I$(INCDIR) -I$(INCDIR)/nc -I$(KERNELDIR)/include

.PHONY: all clean

all: $(MD_TARGETS)

$(BUILDDIR)/%.ko: %.c $(MAKEFILE)
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CC) $(INCLUDE) $(KCFLAGS) -g0 -c $< -o $@
	@echo "XCC" $(notdir $<)

$(BUILDDIR)/%.ko: %.cc $(MAKEFILE)
	@mkdir -p $(BUILDDIR)/$(dir $<)
	$(Q)$(CXX) $(INCLUDE) $(KCXXFLAGS) -g0 -c $< -o $@
	@echo "XCXX" $(notdir $<)
