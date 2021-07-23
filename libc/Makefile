include ../rules.mk

SOURCE := \
	./crt0.S \
	./crti.S \
	./crtn.S \
	./ctype.c \
	./entry.c \
	./errno.c \
	./fstdio.c \
	./fstdio_unlocked.c \
	./getopt.c \
	./locale.c \
	./malloc.c \
	./nightingale.c \
	./setjmp.S \
	./signal.c \
	./stdio.c \
	./stdlib.c \
	./string.c \
	./strtod.c \
	./syscall.c \
	./syscalls.c \
	./time.c \
	./todo.c \
	./unistd.c \
	./vector.c \
	./x86_64/nightingale.c \

OBJECTS := $(SOURCE:=.libc.o)

libc.a: $(OBJECTS)
	$(USER_AR)

%.c.libc.o: %.c
	$(USER_COMPILE)

%.S.libc.o: %.S
	$(USER_COMPILE)

clean:
	rm -f $(OBJECTS)
	rm -f libc.a

