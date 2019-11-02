# vim: noet ts=8 sw=8 sts=8

SRCDIR		= .

INCLUDE		= -I$(INCDIR) -I$(INCDIR)/nc -I$(INCDIR)/nx

ifeq ($(NG), 1)
BUILDDIR	= $(shell pwd)/../build$(ARCH)/libknx
CXXFLAGS	= $(KCXXFLAGS) -D_NG=1
LIBNXSRC	= "" # TODO: support libknx
CXXSTR		= "XCXX"
else
BUILDDIR	= $(shell pwd)/../build$(ARCH)/libcx
LIBNXSRC	= $(shell find . -name "[^_]*.cc")
CXXSTR		= "HCXX"
endif

LIBNXOBJ	= $(addprefix $(BUILDDIR)/, $(addsuffix .o,$(LIBNXSRC)))

MAKEFILE	= Makefile

.PHONY: all clean
.PRECIOUS: $(BUILDDIR)/%.c.o

all: $(LIBNX)

include $(shell find $(BUILDDIR) -name "*.d" $(N))

$(BUILDDIR)/%.cc.o: %.cc
	@mkdir -p $(BUILDDIR)/$(dir $<)
	@echo $(CXXSTR) $(notdir $<)
	$(Q)$(CXX) -MD -MF $(BUILDDIR)/$<.d $(INCLUDE) $(CXXFLAGS) -c $< -o $@

$(LIBKNX): $(LIBNXOBJ)
	@echo "AR" $(notdir $(LIBKNX))
	$(Q)ar rcs $(LIBKNX) $(LIBNXOBJ)

$(LIBNX): $(LIBNXOBJ)
	@echo "AR" $(notdir $(LIBNX))
	$(Q)ar rcs $(LIBNX) $(LIBNXOBJ)

clean:
	rm -f $(BUILDDIR)/*.o
	rm -f $(INITFS)
	rm -f $(LIBNX)

