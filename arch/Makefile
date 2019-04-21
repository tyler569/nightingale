# vim: noet ts=8 sw=8 sts=8

TARGET		= $(BUILDDIR)/lib$(ARCH).a
MODULE		= arch

MD		= $(BUILDDIR)/$(MODULE)
MAKEFILE	= Makefile

INCLUDE		= -I. -I../include/arch -I ../include/arch/x86

ifeq ($(ARCH), X86_64)
CSRC		= $(shell find . -name '*.c' | grep -v 32)
ASRC		= $(shell find . -name '*.asm' | grep -v 32)
else ifeq ($(ARCH), I686)
CSRC		= $(shell find . -name '*.c' | grep -v 64)
ASRC		= $(shell find . -name '*.asm' | grep -v 64)
endif


COBJ		= $(addprefix $(MD)/, $(CSRC:.c=.c.o))
AOBJ		= $(addprefix $(MD)/, $(ASRC:.asm=.asm.o))

OBJECTS		= $(AOBJ) $(COBJ)

.PHONY: all clean

all: $(TARGET)

include $(shell find $(MD) -name "*.d")

$(TARGET): $(OBJECTS) $(MAKEFILE)
	ar rcs -o $(TARGET) $(OBJECTS)
	rm -f $(KERNEL)tmp*

%.asm: 
	# stop it complaining about circular dependancy because %: %.o

$(MD)/%.asm.o: %.asm $(MAKEFILE)
	mkdir -p $(MD)/$(dir $<)
	$(AS) $(KASFLAGS) $< -o $@

$(MD)/%.c.o: %.c %.h $(MAKEFILE)
	@mkdir -p $(MD)/$(dir $<)
	$(CC) -MD -MF $(MD)/$<.d $(INCLUDE) $(KCFLAGS) -c $< -o $@

$(MD)/%.c.o: %.c $(MAKEFILE)
	@mkdir -p $(MD)/$(dir $<)
	$(CC) -MD -MF $(MD)/$<.d $(INCLUDE) $(KCFLAGS) -c $< -o $@

clean:
	rm -f $(KERNEL)

