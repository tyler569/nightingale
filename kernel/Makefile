# vim: noet ts=8 sw=8 sts=8

TARGET		= $(BUILDDIR)/ngk

MODULE		= kernel
MD		= $(BUILDDIR)/$(MODULE)
MAKEFILE	= Makefile

GENERIC_CSRC	= $(wildcard *.c) $(wildcard net/*.c)

ifeq ($(ARCH), X86_64)
ARCH_CSRC	= $(wildcard x86/*.c x86/64/*.c)
ARCH_ASRC	= $(wildcard x86/64/*.asm)
else ifeq ($(ARCH), I686)
ARCH_CSRC	= $(wildcard x86/*.c x86/32/*.c)
ARCH_ASRC	= $(wildcard x86/32/*.asm)
endif

CSRC		= $(GENERIC_CSRC) $(ARCH_CSRC)
ASRC		= $(ARCH_ASRC)

COBJ		= $(addprefix $(MD)/, $(CSRC:.c=.c.o))
AOBJ		= $(addprefix $(MD)/, $(ASRC:.asm=.asm.o))

OBJECTS		= $(AOBJ) $(COBJ)

INCLUDE		= -Iinclude -I$(INCDIR) -I$(INCDIR)/nc -I../linker/include

.PHONY: all clean

all: $(TARGET)

include $(shell find $(MD) -name "*.d" $(N))

# $(TARGET): $(OBJECTS) $(MAKEFILE)
# 	$(Q)ar rcs -o $(TARGET) $(OBJECTS)
# 	$(Q)rm -f $(KERNEL)tmp*
# 	@echo "AR" $(notdir $(TARGET))

$(LIBKNC):
$(LINKER):

$(TARGET): $(OBJECTS) $(MAKEFILE) $(LIBKNC) $(LINKER)
	@echo "LD" $(notdir $(TARGET))
	$(Q)$(LD) $(KLDFLAGS) -o $(TARGET) $(OBJECTS) -llinker -lknc -lgcc

%.asm: 
	# stop it complaining about circular dependancy because %: %.o

$(MD)/%.asm.o: %.asm $(MAKEFILE)
	@echo "AS" $(notdir $<)
	$(Q)mkdir -p $(MD)/$(dir $<)
	$(Q)$(AS) $(KASFLAGS) $< -o $@

$(MD)/%.c.o: %.c $(MAKEFILE)
	@echo "XCC" $(notdir $<)
	$(Q)mkdir -p $(MD)/$(dir $<)
	$(Q)$(CC) -MD -MF $(MD)/$<.d $(INCLUDE) $(KCFLAGS) -c $< -o $@

clean:
	rm -f $(KERNEL)

