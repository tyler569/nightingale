CFLAGS := \
	-std=c11 \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-g \
	-O2 \
	-D__nightingale__=1 \
	-ffreestanding \
	-nostdlib \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-function \
	-Wno-sign-compare \
	-Wno-address-of-packed-member \

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
	ar rcs $@ $^

%.c.libc.o: %.c
	clang -target x86_64-unknown-none -I../include $(CFLAGS) -o $@ -c $<

%.S.libc.o: %.S
	clang -target x86_64-unknown-none -I../include $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJECTS)
	rm -f libc.a

