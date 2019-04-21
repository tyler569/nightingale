# vim: noet ts=8 sw=8 sts=8

TARGET		= $(BUILDDIR)/libds.a
MODULE		= ds

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

