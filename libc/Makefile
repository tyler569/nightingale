# vim: noet ts=8 sw=8 sts=8

CC		= x86_64-elf-gcc
LD		= $(CC)

CFLAGS		= -O0 -g -ffreestanding -nostdlib -I../include

SRCDIR		= .
BUILDDIR 	= ../build/libc

CSRC		= $(shell find $(SRCDIR) -name '*.c')
COBJ		= $(CSRC:.c=.c.o)

OBJECTS		= $(addprefix $(BUILDDIR)/, $(COBJ))

LDFLAGS		=
LDLIBS		= -lgcc

TARGET		= libc.a

MAKEFILE	= Makefile

$(BUILDDIR)/%.c.o: %.c %.h $(MAKEFILE)
	mkdir -p $(BUILDDIR)/$<
	$(CC) -I$(SRCDIR) -MD -MF $(BUILDDIR)/$<.d $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	ar rcs $(TARGET) $(OBJECTS)

clean:
	rm -f $(TARGET)
