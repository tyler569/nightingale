# vim: noet ts=8 sw=8 sts=8

SRCDIR		= .

INCLUDE		= -I$(INCDIR) -I$(INCDIR)/nc -I$(INCDIR)/nx

ifeq ($(NG), 1)
MD		= $(BUILDDIR)/libknx
CXXFLAGS	= $(KCXXFLAGS) -D_NG=1
LIBNXSRC	= "" # TODO: support libknx
CXXSTR		= "XCXX"
else
MD		= $(BUILDDIR)/libnx
LIBNXSRC	= $(shell find . -name "[^_]*.cc")
CXXSTR		= "HCXX"
endif

LIBNXOBJ	= $(addprefix $(MD)/, $(addsuffix .o,$(LIBNXSRC)))

MAKEFILE	= Makefile

.PHONY: all clean
.PRECIOUS: $(MD)/%.c.o

all: $(LIBNX)

include $(shell find $(MD) -name "*.d" $(N))

$(MD)/%.cc.o: %.cc
	@mkdir -p $(MD)/$(dir $<)
	@echo $(CXXSTR) $(notdir $<)
	$(Q)$(CXX) -MD -MF $(MD)/$<.d $(INCLUDE) $(CXXFLAGS) -c $< -o $@

$(LIBKNX): $(LIBNXOBJ)
	@echo "AR" $(notdir $(LIBKNX))
	$(Q)ar rcs $(LIBKNX) $(LIBNXOBJ)

$(LIBNX): $(LIBNXOBJ)
	@echo "AR" $(notdir $(LIBNX))
	$(Q)ar rcs $(LIBNX) $(LIBNXOBJ)

clean:
	rm -f $(MD)/*.o
	rm -f $(INITFS)
	rm -f $(LIBNX)

