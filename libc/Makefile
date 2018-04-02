
CC = x86_64-elf-gcc
LD = $(CC)

CFLAGS = -ffreestanding -nostdlib

SRCDIR = .
CSRC = $(shell find $(SRCDIR) -name '*.c')
COBJ = $(CSRC:.c=.c.o)

OBJECTS = $(COBJ)
LDFLAGS = 
LDLIBS = -lgcc

TARGET = libc.a

MAKEFILE = Makefile

%.c.o: %.c %.h $(MAKEFILE)
	$(CC) -I$(SRCDIR) -MD -MF $<.d $(CFLAGS) -c $< -o $@

$(TARGET): $(COBJ)
	ar rcs $(TARGET) $(OBJECTS)

clean:
	rm $(shell find $(SRCDIR) -name '*.o')
	rm $(TARGET)
