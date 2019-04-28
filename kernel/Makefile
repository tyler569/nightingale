# vim: noet ts=8 sw=8 sts=8

TARGET		= $(BUILDDIR)/libnightingale.a
MODULE		= kernel

MD		= $(BUILDDIR)/$(MODULE)
MAKEFILE	= Makefile

INCLUDE		= -I.

CSRC		= $(shell find . -name '*.c')
ASRC		= $(shell find . -name '*.asm')

COBJ		= $(addprefix $(MD)/, $(CSRC:.c=.c.o))
AOBJ		= $(addprefix $(MD)/, $(ASRC:.asm=.asm.o))

OBJECTS		= $(AOBJ) $(COBJ)

.PHONY: all clean

all: $(TARGET)

include $(shell find $(MD) -name "*.d" $(N))

$(TARGET): $(OBJECTS) $(MAKEFILE)
	$(Q)ar rcs -o $(TARGET) $(OBJECTS)
	$(Q)rm -f $(KERNEL)tmp*
	@echo "AR" $(notdir $(TARGET))

%.asm: 
	# stop it complaining about circular dependancy because %: %.o

$(MD)/%.asm.o: %.asm $(MAKEFILE)
	$(Q)mkdir -p $(MD)/$(dir $<)
	$(Q)$(AS) $(KASFLAGS) $< -o $@
	@echo "AS" $(notdir $<)

$(MD)/%.c.o: %.c $(MAKEFILE)
	$(Q)mkdir -p $(MD)/$(dir $<)
	$(Q)$(CC) -MD -MF $(MD)/$<.d $(INCLUDE) $(KCFLAGS) -c $< -o $@
	@echo "XCC" $(notdir $<)

clean:
	rm -f $(KERNEL)

