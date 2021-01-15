.PHONY: mp_all mp_clean mp_install

mp_all: $(MP_ALL_TARGETS)

build-x86_64/libc.a: build-x86_64/libc/crt0.o build-x86_64/libc/crti.o build-x86_64/libc/crtn.o build-x86_64/libc/ctype.o build-x86_64/libc/entry.o build-x86_64/libc/errno.o build-x86_64/libc/fstdio.o build-x86_64/libc/fstdio_unlocked.o build-x86_64/libc/getopt.o build-x86_64/libc/locale.o build-x86_64/libc/malloc.o build-x86_64/libc/nightingale.o build-x86_64/libc/setjmp.o build-x86_64/libc/signal.o build-x86_64/libc/stdio.o build-x86_64/libc/stdlib.o build-x86_64/libc/string.o build-x86_64/libc/strtod.o build-x86_64/libc/syscall.o build-x86_64/libc/syscalls.o build-x86_64/libc/time.o build-x86_64/libc/todo.o build-x86_64/libc/unistd.o build-x86_64/libc/vector.o build-x86_64/libc/x86_64/nightingale.o 
	$(call MP_INFO,AR	libc.a)
	@ar rcs -o build-x86_64/libc.a build-x86_64/libc/crt0.o build-x86_64/libc/crti.o build-x86_64/libc/crtn.o build-x86_64/libc/ctype.o build-x86_64/libc/entry.o build-x86_64/libc/errno.o build-x86_64/libc/fstdio.o build-x86_64/libc/fstdio_unlocked.o build-x86_64/libc/getopt.o build-x86_64/libc/locale.o build-x86_64/libc/malloc.o build-x86_64/libc/nightingale.o build-x86_64/libc/setjmp.o build-x86_64/libc/signal.o build-x86_64/libc/stdio.o build-x86_64/libc/stdlib.o build-x86_64/libc/string.o build-x86_64/libc/strtod.o build-x86_64/libc/syscall.o build-x86_64/libc/syscalls.o build-x86_64/libc/time.o build-x86_64/libc/todo.o build-x86_64/libc/unistd.o build-x86_64/libc/vector.o build-x86_64/libc/x86_64/nightingale.o
build-x86_64/libc/crt0.o: libc/crt0.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,AS	crt0.o)
	@gcc  -MD -MF dep/libc/crt0.d -c libc/crt0.S -o build-x86_64/libc/crt0.o
build-x86_64/libc/crti.o: libc/crti.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,AS	crti.o)
	@gcc  -MD -MF dep/libc/crti.d -c libc/crti.S -o build-x86_64/libc/crti.o
build-x86_64/libc/crtn.o: libc/crtn.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,AS	crtn.o)
	@gcc  -MD -MF dep/libc/crtn.d -c libc/crtn.S -o build-x86_64/libc/crtn.o
build-x86_64/libc/ctype.o: libc/ctype.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	ctype.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/ctype.d -c libc/ctype.c -o build-x86_64/libc/ctype.o
build-x86_64/libc/entry.o: libc/entry.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	entry.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/entry.d -c libc/entry.c -o build-x86_64/libc/entry.o
build-x86_64/libc/errno.o: libc/errno.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	errno.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/errno.d -c libc/errno.c -o build-x86_64/libc/errno.o
build-x86_64/libc/fstdio.o: libc/fstdio.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	fstdio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/fstdio.d -c libc/fstdio.c -o build-x86_64/libc/fstdio.o
build-x86_64/libc/fstdio_unlocked.o: libc/fstdio_unlocked.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	fstdio_unlocked.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/fstdio_unlocked.d -c libc/fstdio_unlocked.c -o build-x86_64/libc/fstdio_unlocked.o
build-x86_64/libc/getopt.o: libc/getopt.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	getopt.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/getopt.d -c libc/getopt.c -o build-x86_64/libc/getopt.o
build-x86_64/libc/locale.o: libc/locale.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	locale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/locale.d -c libc/locale.c -o build-x86_64/libc/locale.o
build-x86_64/libc/malloc.o: libc/malloc.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	malloc.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/malloc.d -c libc/malloc.c -o build-x86_64/libc/malloc.o
build-x86_64/libc/nightingale.o: libc/nightingale.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	nightingale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/nightingale.d -c libc/nightingale.c -o build-x86_64/libc/nightingale.o
build-x86_64/libc/setjmp.o: libc/setjmp.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,AS	setjmp.o)
	@gcc  -MD -MF dep/libc/setjmp.d -c libc/setjmp.S -o build-x86_64/libc/setjmp.o
build-x86_64/libc/signal.o: libc/signal.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	signal.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/signal.d -c libc/signal.c -o build-x86_64/libc/signal.o
build-x86_64/libc/stdio.o: libc/stdio.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	stdio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/stdio.d -c libc/stdio.c -o build-x86_64/libc/stdio.o
build-x86_64/libc/stdlib.o: libc/stdlib.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	stdlib.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/stdlib.d -c libc/stdlib.c -o build-x86_64/libc/stdlib.o
build-x86_64/libc/string.o: libc/string.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	string.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/string.d -c libc/string.c -o build-x86_64/libc/string.o
build-x86_64/libc/strtod.o: libc/strtod.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	strtod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/strtod.d -c libc/strtod.c -o build-x86_64/libc/strtod.o
build-x86_64/libc/syscall.o: libc/syscall.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	syscall.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/syscall.d -c libc/syscall.c -o build-x86_64/libc/syscall.o
build-x86_64/libc/syscalls.o: libc/syscalls.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	syscalls.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/syscalls.d -c libc/syscalls.c -o build-x86_64/libc/syscalls.o
build-x86_64/libc/time.o: libc/time.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	time.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/time.d -c libc/time.c -o build-x86_64/libc/time.o
build-x86_64/libc/todo.o: libc/todo.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	todo.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/todo.d -c libc/todo.c -o build-x86_64/libc/todo.o
build-x86_64/libc/unistd.o: libc/unistd.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	unistd.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/unistd.d -c libc/unistd.c -o build-x86_64/libc/unistd.o
build-x86_64/libc/vector.o: libc/vector.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(call MP_INFO,CC	vector.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/vector.d -c libc/vector.c -o build-x86_64/libc/vector.o
build-x86_64/libc/x86_64/nightingale.o: libc/x86_64/nightingale.c
	@mkdir -p build-x86_64/libc/x86_64
	@mkdir -p dep/libc/x86_64
	$(call MP_INFO,CC	nightingale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libc/x86_64/nightingale.d -c libc/x86_64/nightingale.c -o build-x86_64/libc/x86_64/nightingale.o
build-x86_64/libm.a: build-x86_64/libm/complex.o build-x86_64/libm/double.o 
	$(call MP_INFO,AR	libm.a)
	@ar rcs -o build-x86_64/libm.a build-x86_64/libm/complex.o build-x86_64/libm/double.o
build-x86_64/libm/complex.o: libm/complex.c
	@mkdir -p build-x86_64/libm
	@mkdir -p dep/libm
	$(call MP_INFO,CC	complex.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libm/complex.d -c libm/complex.c -o build-x86_64/libm/complex.o
build-x86_64/libm/double.o: libm/double.c
	@mkdir -p build-x86_64/libm
	@mkdir -p dep/libm
	$(call MP_INFO,CC	double.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libm/double.d -c libm/double.c -o build-x86_64/libm/double.o
build-x86_64/libelf.a: build-x86_64/libelf/elf-ng.o 
	$(call MP_INFO,AR	libelf.a)
	@ar rcs -o build-x86_64/libelf.a build-x86_64/libelf/elf-ng.o
build-x86_64/libelf/elf-ng.o: linker/elf-ng.c
	@mkdir -p build-x86_64/libelf
	@mkdir -p dep/libelf
	$(call MP_INFO,CC	elf-ng.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/libelf/elf-ng.d -c linker/elf-ng.c -o build-x86_64/libelf/elf-ng.o
build-x86_64/libc.so: build-x86_64/libc_so/crt0.o build-x86_64/libc_so/crti.o build-x86_64/libc_so/crtn.o build-x86_64/libc_so/ctype.o build-x86_64/libc_so/entry.o build-x86_64/libc_so/errno.o build-x86_64/libc_so/fstdio.o build-x86_64/libc_so/fstdio_unlocked.o build-x86_64/libc_so/getopt.o build-x86_64/libc_so/locale.o build-x86_64/libc_so/malloc.o build-x86_64/libc_so/nightingale.o build-x86_64/libc_so/setjmp.o build-x86_64/libc_so/signal.o build-x86_64/libc_so/stdio.o build-x86_64/libc_so/stdlib.o build-x86_64/libc_so/string.o build-x86_64/libc_so/strtod.o build-x86_64/libc_so/syscall.o build-x86_64/libc_so/syscalls.o build-x86_64/libc_so/time.o build-x86_64/libc_so/todo.o build-x86_64/libc_so/unistd.o build-x86_64/libc_so/vector.o build-x86_64/libc_so/x86_64/nightingale.o 
	$(call MP_INFO,LD	libc.so)
	@x86_64-nightingale-gcc -nostdlib -fpic -shared -g -o build-x86_64/libc.so build-x86_64/libc_so/crt0.o build-x86_64/libc_so/crti.o build-x86_64/libc_so/crtn.o build-x86_64/libc_so/ctype.o build-x86_64/libc_so/entry.o build-x86_64/libc_so/errno.o build-x86_64/libc_so/fstdio.o build-x86_64/libc_so/fstdio_unlocked.o build-x86_64/libc_so/getopt.o build-x86_64/libc_so/locale.o build-x86_64/libc_so/malloc.o build-x86_64/libc_so/nightingale.o build-x86_64/libc_so/setjmp.o build-x86_64/libc_so/signal.o build-x86_64/libc_so/stdio.o build-x86_64/libc_so/stdlib.o build-x86_64/libc_so/string.o build-x86_64/libc_so/strtod.o build-x86_64/libc_so/syscall.o build-x86_64/libc_so/syscalls.o build-x86_64/libc_so/time.o build-x86_64/libc_so/todo.o build-x86_64/libc_so/unistd.o build-x86_64/libc_so/vector.o build-x86_64/libc_so/x86_64/nightingale.o 
build-x86_64/libc_so/crt0.o: libc/crt0.S
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,AS	crt0.o)
	@gcc  -MD -MF dep/libc_so/crt0.d -c libc/crt0.S -o build-x86_64/libc_so/crt0.o
build-x86_64/libc_so/crti.o: libc/crti.S
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,AS	crti.o)
	@gcc  -MD -MF dep/libc_so/crti.d -c libc/crti.S -o build-x86_64/libc_so/crti.o
build-x86_64/libc_so/crtn.o: libc/crtn.S
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,AS	crtn.o)
	@gcc  -MD -MF dep/libc_so/crtn.d -c libc/crtn.S -o build-x86_64/libc_so/crtn.o
build-x86_64/libc_so/ctype.o: libc/ctype.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	ctype.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/ctype.d -c libc/ctype.c -o build-x86_64/libc_so/ctype.o
build-x86_64/libc_so/entry.o: libc/entry.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	entry.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/entry.d -c libc/entry.c -o build-x86_64/libc_so/entry.o
build-x86_64/libc_so/errno.o: libc/errno.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	errno.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/errno.d -c libc/errno.c -o build-x86_64/libc_so/errno.o
build-x86_64/libc_so/fstdio.o: libc/fstdio.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	fstdio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/fstdio.d -c libc/fstdio.c -o build-x86_64/libc_so/fstdio.o
build-x86_64/libc_so/fstdio_unlocked.o: libc/fstdio_unlocked.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	fstdio_unlocked.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/fstdio_unlocked.d -c libc/fstdio_unlocked.c -o build-x86_64/libc_so/fstdio_unlocked.o
build-x86_64/libc_so/getopt.o: libc/getopt.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	getopt.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/getopt.d -c libc/getopt.c -o build-x86_64/libc_so/getopt.o
build-x86_64/libc_so/locale.o: libc/locale.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	locale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/locale.d -c libc/locale.c -o build-x86_64/libc_so/locale.o
build-x86_64/libc_so/malloc.o: libc/malloc.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	malloc.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/malloc.d -c libc/malloc.c -o build-x86_64/libc_so/malloc.o
build-x86_64/libc_so/nightingale.o: libc/nightingale.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	nightingale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/nightingale.d -c libc/nightingale.c -o build-x86_64/libc_so/nightingale.o
build-x86_64/libc_so/setjmp.o: libc/setjmp.S
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,AS	setjmp.o)
	@gcc  -MD -MF dep/libc_so/setjmp.d -c libc/setjmp.S -o build-x86_64/libc_so/setjmp.o
build-x86_64/libc_so/signal.o: libc/signal.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	signal.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/signal.d -c libc/signal.c -o build-x86_64/libc_so/signal.o
build-x86_64/libc_so/stdio.o: libc/stdio.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	stdio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/stdio.d -c libc/stdio.c -o build-x86_64/libc_so/stdio.o
build-x86_64/libc_so/stdlib.o: libc/stdlib.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	stdlib.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/stdlib.d -c libc/stdlib.c -o build-x86_64/libc_so/stdlib.o
build-x86_64/libc_so/string.o: libc/string.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	string.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/string.d -c libc/string.c -o build-x86_64/libc_so/string.o
build-x86_64/libc_so/strtod.o: libc/strtod.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	strtod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/strtod.d -c libc/strtod.c -o build-x86_64/libc_so/strtod.o
build-x86_64/libc_so/syscall.o: libc/syscall.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	syscall.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/syscall.d -c libc/syscall.c -o build-x86_64/libc_so/syscall.o
build-x86_64/libc_so/syscalls.o: libc/syscalls.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	syscalls.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/syscalls.d -c libc/syscalls.c -o build-x86_64/libc_so/syscalls.o
build-x86_64/libc_so/time.o: libc/time.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	time.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/time.d -c libc/time.c -o build-x86_64/libc_so/time.o
build-x86_64/libc_so/todo.o: libc/todo.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	todo.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/todo.d -c libc/todo.c -o build-x86_64/libc_so/todo.o
build-x86_64/libc_so/unistd.o: libc/unistd.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	unistd.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/unistd.d -c libc/unistd.c -o build-x86_64/libc_so/unistd.o
build-x86_64/libc_so/vector.o: libc/vector.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(call MP_INFO,CC	vector.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/vector.d -c libc/vector.c -o build-x86_64/libc_so/vector.o
build-x86_64/libc_so/x86_64/nightingale.o: libc/x86_64/nightingale.c
	@mkdir -p build-x86_64/libc_so/x86_64
	@mkdir -p dep/libc_so/x86_64
	$(call MP_INFO,CC	nightingale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/x86_64/nightingale.d -c libc/x86_64/nightingale.c -o build-x86_64/libc_so/x86_64/nightingale.o
build-x86_64/libk.a: build-x86_64/libk/ctype.o build-x86_64/libk/errno.o build-x86_64/libk/malloc.o build-x86_64/libk/setjmp.o build-x86_64/libk/signal.o build-x86_64/libk/stdio.o build-x86_64/libk/stdlib.o build-x86_64/libk/string.o build-x86_64/libk/x86_64/nightingale.o 
	$(call MP_INFO,AR	libk.a)
	@ar rcs -o build-x86_64/libk.a build-x86_64/libk/ctype.o build-x86_64/libk/errno.o build-x86_64/libk/malloc.o build-x86_64/libk/setjmp.o build-x86_64/libk/signal.o build-x86_64/libk/stdio.o build-x86_64/libk/stdlib.o build-x86_64/libk/string.o build-x86_64/libk/x86_64/nightingale.o
build-x86_64/libk/ctype.o: libc/ctype.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(call MP_INFO,CC	ctype.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/libk/ctype.d -c libc/ctype.c -o build-x86_64/libk/ctype.o
build-x86_64/libk/errno.o: libc/errno.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(call MP_INFO,CC	errno.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/libk/errno.d -c libc/errno.c -o build-x86_64/libk/errno.o
build-x86_64/libk/malloc.o: libc/malloc.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(call MP_INFO,CC	malloc.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/libk/malloc.d -c libc/malloc.c -o build-x86_64/libk/malloc.o
build-x86_64/libk/setjmp.o: libc/setjmp.S
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(call MP_INFO,AS	setjmp.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/libk/setjmp.d -c libc/setjmp.S -o build-x86_64/libk/setjmp.o
build-x86_64/libk/signal.o: libc/signal.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(call MP_INFO,CC	signal.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/libk/signal.d -c libc/signal.c -o build-x86_64/libk/signal.o
build-x86_64/libk/stdio.o: libc/stdio.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(call MP_INFO,CC	stdio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/libk/stdio.d -c libc/stdio.c -o build-x86_64/libk/stdio.o
build-x86_64/libk/stdlib.o: libc/stdlib.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(call MP_INFO,CC	stdlib.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/libk/stdlib.d -c libc/stdlib.c -o build-x86_64/libk/stdlib.o
build-x86_64/libk/string.o: libc/string.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(call MP_INFO,CC	string.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/libk/string.d -c libc/string.c -o build-x86_64/libk/string.o
build-x86_64/libk/x86_64/nightingale.o: libc/x86_64/nightingale.c
	@mkdir -p build-x86_64/libk/x86_64
	@mkdir -p dep/libk/x86_64
	$(call MP_INFO,CC	nightingale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/libk/x86_64/nightingale.d -c libc/x86_64/nightingale.c -o build-x86_64/libk/x86_64/nightingale.o
build-x86_64/ngk.elf: build-x86_64/fs/char_devices.o build-x86_64/fs/directory.o build-x86_64/fs/fs.o build-x86_64/fs/membuf.o build-x86_64/fs/pipe.o build-x86_64/fs/procfs.o build-x86_64/fs/socket.o build-x86_64/kernel/debug.o build-x86_64/kernel/dmgr.o build-x86_64/kernel/elf.o build-x86_64/kernel/exec.o build-x86_64/kernel/irq.o build-x86_64/kernel/main.o build-x86_64/kernel/mman.o build-x86_64/kernel/mod.o build-x86_64/kernel/multiboot.o build-x86_64/kernel/pci.o build-x86_64/kernel/pmm.o build-x86_64/kernel/rand.o build-x86_64/kernel/ringbuf.o build-x86_64/kernel/serial.o build-x86_64/kernel/signal.o build-x86_64/kernel/spalloc.o build-x86_64/kernel/string.o build-x86_64/kernel/sync.o build-x86_64/kernel/sync_testbed.o build-x86_64/kernel/syscall.o build-x86_64/kernel/tarfs.o build-x86_64/kernel/tests.o build-x86_64/kernel/thread.o build-x86_64/kernel/timer.o build-x86_64/kernel/trace.o build-x86_64/kernel/tty.o build-x86_64/kernel/ubsan.o build-x86_64/kernel/uname.o build-x86_64/linker/elf-ng.o build-x86_64/linker/modld.o build-x86_64/x86/acpi.o build-x86_64/x86/apic.o build-x86_64/x86/boot.o build-x86_64/x86/cpu.o build-x86_64/x86/halt.o build-x86_64/x86/interrupt.o build-x86_64/x86/isrs.o build-x86_64/x86/pic.o build-x86_64/x86/pit.o build-x86_64/x86/uart.o build-x86_64/x86/vmm.o build-x86_64/libk.a
	$(call MP_INFO,LD	ngk.elf)
	@x86_64-nightingale-gcc -nostdlib -Tx86/link_hh.ld -zmax-page-size=0x1000 -g -Lbuild-x86_64 -o build-x86_64/ngk.elf build-x86_64/fs/char_devices.o build-x86_64/fs/directory.o build-x86_64/fs/fs.o build-x86_64/fs/membuf.o build-x86_64/fs/pipe.o build-x86_64/fs/procfs.o build-x86_64/fs/socket.o build-x86_64/kernel/debug.o build-x86_64/kernel/dmgr.o build-x86_64/kernel/elf.o build-x86_64/kernel/exec.o build-x86_64/kernel/irq.o build-x86_64/kernel/main.o build-x86_64/kernel/mman.o build-x86_64/kernel/mod.o build-x86_64/kernel/multiboot.o build-x86_64/kernel/pci.o build-x86_64/kernel/pmm.o build-x86_64/kernel/rand.o build-x86_64/kernel/ringbuf.o build-x86_64/kernel/serial.o build-x86_64/kernel/signal.o build-x86_64/kernel/spalloc.o build-x86_64/kernel/string.o build-x86_64/kernel/sync.o build-x86_64/kernel/sync_testbed.o build-x86_64/kernel/syscall.o build-x86_64/kernel/tarfs.o build-x86_64/kernel/tests.o build-x86_64/kernel/thread.o build-x86_64/kernel/timer.o build-x86_64/kernel/trace.o build-x86_64/kernel/tty.o build-x86_64/kernel/ubsan.o build-x86_64/kernel/uname.o build-x86_64/linker/elf-ng.o build-x86_64/linker/modld.o build-x86_64/x86/acpi.o build-x86_64/x86/apic.o build-x86_64/x86/boot.o build-x86_64/x86/cpu.o build-x86_64/x86/halt.o build-x86_64/x86/interrupt.o build-x86_64/x86/isrs.o build-x86_64/x86/pic.o build-x86_64/x86/pit.o build-x86_64/x86/uart.o build-x86_64/x86/vmm.o -lk -lgcc
build-x86_64/fs/char_devices.o: fs/char_devices.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(call MP_INFO,CC	char_devices.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/fs/char_devices.d -c fs/char_devices.c -o build-x86_64/fs/char_devices.o
build-x86_64/fs/directory.o: fs/directory.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(call MP_INFO,CC	directory.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/fs/directory.d -c fs/directory.c -o build-x86_64/fs/directory.o
build-x86_64/fs/fs.o: fs/fs.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(call MP_INFO,CC	fs.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/fs/fs.d -c fs/fs.c -o build-x86_64/fs/fs.o
build-x86_64/fs/membuf.o: fs/membuf.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(call MP_INFO,CC	membuf.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/fs/membuf.d -c fs/membuf.c -o build-x86_64/fs/membuf.o
build-x86_64/fs/pipe.o: fs/pipe.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(call MP_INFO,CC	pipe.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/fs/pipe.d -c fs/pipe.c -o build-x86_64/fs/pipe.o
build-x86_64/fs/procfs.o: fs/procfs.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(call MP_INFO,CC	procfs.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/fs/procfs.d -c fs/procfs.c -o build-x86_64/fs/procfs.o
build-x86_64/fs/socket.o: fs/socket.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(call MP_INFO,CC	socket.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/fs/socket.d -c fs/socket.c -o build-x86_64/fs/socket.o
build-x86_64/kernel/debug.o: kernel/debug.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	debug.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/debug.d -c kernel/debug.c -o build-x86_64/kernel/debug.o
build-x86_64/kernel/dmgr.o: kernel/dmgr.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	dmgr.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/dmgr.d -c kernel/dmgr.c -o build-x86_64/kernel/dmgr.o
build-x86_64/kernel/elf.o: kernel/elf.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	elf.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/elf.d -c kernel/elf.c -o build-x86_64/kernel/elf.o
build-x86_64/kernel/exec.o: kernel/exec.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	exec.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/exec.d -c kernel/exec.c -o build-x86_64/kernel/exec.o
build-x86_64/kernel/irq.o: kernel/irq.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	irq.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/irq.d -c kernel/irq.c -o build-x86_64/kernel/irq.o
build-x86_64/kernel/main.o: kernel/main.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	main.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/main.d -c kernel/main.c -o build-x86_64/kernel/main.o
build-x86_64/kernel/mman.o: kernel/mman.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	mman.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/mman.d -c kernel/mman.c -o build-x86_64/kernel/mman.o
build-x86_64/kernel/mod.o: kernel/mod.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	mod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/mod.d -c kernel/mod.c -o build-x86_64/kernel/mod.o
build-x86_64/kernel/multiboot.o: kernel/multiboot.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	multiboot.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/multiboot.d -c kernel/multiboot.c -o build-x86_64/kernel/multiboot.o
build-x86_64/kernel/pci.o: kernel/pci.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	pci.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/pci.d -c kernel/pci.c -o build-x86_64/kernel/pci.o
build-x86_64/kernel/pmm.o: kernel/pmm.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	pmm.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/pmm.d -c kernel/pmm.c -o build-x86_64/kernel/pmm.o
build-x86_64/kernel/rand.o: kernel/rand.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	rand.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/rand.d -c kernel/rand.c -o build-x86_64/kernel/rand.o
build-x86_64/kernel/ringbuf.o: kernel/ringbuf.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	ringbuf.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/ringbuf.d -c kernel/ringbuf.c -o build-x86_64/kernel/ringbuf.o
build-x86_64/kernel/serial.o: kernel/serial.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	serial.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/serial.d -c kernel/serial.c -o build-x86_64/kernel/serial.o
build-x86_64/kernel/signal.o: kernel/signal.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	signal.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/signal.d -c kernel/signal.c -o build-x86_64/kernel/signal.o
build-x86_64/kernel/spalloc.o: kernel/spalloc.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	spalloc.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/spalloc.d -c kernel/spalloc.c -o build-x86_64/kernel/spalloc.o
build-x86_64/kernel/string.o: kernel/string.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	string.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/string.d -c kernel/string.c -o build-x86_64/kernel/string.o
build-x86_64/kernel/sync.o: kernel/sync.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	sync.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/sync.d -c kernel/sync.c -o build-x86_64/kernel/sync.o
build-x86_64/kernel/sync_testbed.o: kernel/sync_testbed.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	sync_testbed.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/sync_testbed.d -c kernel/sync_testbed.c -o build-x86_64/kernel/sync_testbed.o
build-x86_64/kernel/syscall.o: kernel/syscall.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	syscall.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/syscall.d -c kernel/syscall.c -o build-x86_64/kernel/syscall.o
build-x86_64/kernel/tarfs.o: kernel/tarfs.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	tarfs.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/tarfs.d -c kernel/tarfs.c -o build-x86_64/kernel/tarfs.o
build-x86_64/kernel/tests.o: kernel/tests.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	tests.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/tests.d -c kernel/tests.c -o build-x86_64/kernel/tests.o
build-x86_64/kernel/thread.o: kernel/thread.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	thread.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/thread.d -c kernel/thread.c -o build-x86_64/kernel/thread.o
build-x86_64/kernel/timer.o: kernel/timer.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	timer.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/timer.d -c kernel/timer.c -o build-x86_64/kernel/timer.o
build-x86_64/kernel/trace.o: kernel/trace.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	trace.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/trace.d -c kernel/trace.c -o build-x86_64/kernel/trace.o
build-x86_64/kernel/tty.o: kernel/tty.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	tty.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/tty.d -c kernel/tty.c -o build-x86_64/kernel/tty.o
build-x86_64/kernel/ubsan.o: kernel/ubsan.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	ubsan.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/ubsan.d -c kernel/ubsan.c -o build-x86_64/kernel/ubsan.o
build-x86_64/kernel/uname.o: kernel/uname.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(call MP_INFO,CC	uname.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/kernel/uname.d -c kernel/uname.c -o build-x86_64/kernel/uname.o
build-x86_64/linker/elf-ng.o: linker/elf-ng.c
	@mkdir -p build-x86_64/linker
	@mkdir -p dep/linker
	$(call MP_INFO,CC	elf-ng.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/linker/elf-ng.d -c linker/elf-ng.c -o build-x86_64/linker/elf-ng.o
build-x86_64/linker/modld.o: linker/modld.c
	@mkdir -p build-x86_64/linker
	@mkdir -p dep/linker
	$(call MP_INFO,CC	modld.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/linker/modld.d -c linker/modld.c -o build-x86_64/linker/modld.o
build-x86_64/x86/acpi.o: x86/acpi.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,CC	acpi.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/x86/acpi.d -c x86/acpi.c -o build-x86_64/x86/acpi.o
build-x86_64/x86/apic.o: x86/apic.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,CC	apic.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/x86/apic.d -c x86/apic.c -o build-x86_64/x86/apic.o
build-x86_64/x86/boot.o: x86/boot.asm
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,NASM	boot.o)
	@nasm -felf64 x86/boot.asm -o build-x86_64/x86/boot.o
build-x86_64/x86/cpu.o: x86/cpu.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,CC	cpu.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/x86/cpu.d -c x86/cpu.c -o build-x86_64/x86/cpu.o
build-x86_64/x86/halt.o: x86/halt.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,CC	halt.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/x86/halt.d -c x86/halt.c -o build-x86_64/x86/halt.o
build-x86_64/x86/interrupt.o: x86/interrupt.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,CC	interrupt.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/x86/interrupt.d -c x86/interrupt.c -o build-x86_64/x86/interrupt.o
build-x86_64/x86/isrs.o: x86/isrs.asm
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,NASM	isrs.o)
	@nasm -felf64 x86/isrs.asm -o build-x86_64/x86/isrs.o
build-x86_64/x86/pic.o: x86/pic.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,CC	pic.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/x86/pic.d -c x86/pic.c -o build-x86_64/x86/pic.o
build-x86_64/x86/pit.o: x86/pit.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,CC	pit.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/x86/pit.d -c x86/pit.c -o build-x86_64/x86/pit.o
build-x86_64/x86/uart.o: x86/uart.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,CC	uart.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/x86/uart.d -c x86/uart.c -o build-x86_64/x86/uart.o
build-x86_64/x86/vmm.o: x86/vmm.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(call MP_INFO,CC	vmm.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/x86/vmm.d -c x86/vmm.c -o build-x86_64/x86/vmm.o
build-x86_64/crt0.o: build-x86_64/crt/crt0.o
	$(call MP_INFO,CP	crt0.o)
	@cp --preserve=timestamps build-x86_64/crt/crt0.o build-x86_64/crt0.o
build-x86_64/crt/crt0.o: libc/crt0.S
	@mkdir -p build-x86_64/crt
	@mkdir -p dep/crt
	$(call MP_INFO,AS	crt0.o)
	@x86_64-nightingale-gcc  -MD -MF dep/crt/crt0.d -c libc/crt0.S -o build-x86_64/crt/crt0.o
build-x86_64/sh: build-x86_64/sh-/eval.o build-x86_64/sh-/parse.o build-x86_64/sh-/readline.o build-x86_64/sh-/sh.o build-x86_64/sh-/token.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(call MP_INFO,LD	sh)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/sh build-x86_64/sh-/eval.o build-x86_64/sh-/parse.o build-x86_64/sh-/readline.o build-x86_64/sh-/sh.o build-x86_64/sh-/token.o 
build-x86_64/sh-/eval.o: sh/eval.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	$(call MP_INFO,CC	eval.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/sh-/eval.d -c sh/eval.c -o build-x86_64/sh-/eval.o
build-x86_64/sh-/parse.o: sh/parse.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	$(call MP_INFO,CC	parse.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/sh-/parse.d -c sh/parse.c -o build-x86_64/sh-/parse.o
build-x86_64/sh-/readline.o: sh/readline.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	$(call MP_INFO,CC	readline.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/sh-/readline.d -c sh/readline.c -o build-x86_64/sh-/readline.o
build-x86_64/sh-/sh.o: sh/sh.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	$(call MP_INFO,CC	sh.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/sh-/sh.d -c sh/sh.c -o build-x86_64/sh-/sh.o
build-x86_64/sh-/token.o: sh/token.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	$(call MP_INFO,CC	token.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/sh-/token.d -c sh/token.c -o build-x86_64/sh-/token.o
build-x86_64/ld-ng.so: build-x86_64/ld-ng/syscall.o build-x86_64/ld-ng/syscalls.o build-x86_64/ld-ng/elf-ng.o build-x86_64/ld-ng/ldso.o build-x86_64/ld-ng/pltstub.o 
	$(call MP_INFO,LD	ld-ng.so)
	@x86_64-nightingale-gcc -nostdlib -fpic -shared -g -o build-x86_64/ld-ng.so build-x86_64/ld-ng/syscall.o build-x86_64/ld-ng/syscalls.o build-x86_64/ld-ng/elf-ng.o build-x86_64/ld-ng/ldso.o build-x86_64/ld-ng/pltstub.o 
build-x86_64/ld-ng/syscall.o: libc/syscall.c
	@mkdir -p build-x86_64/ld-ng
	@mkdir -p dep/ld-ng
	$(call MP_INFO,CC	syscall.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/ld-ng/syscall.d -c libc/syscall.c -o build-x86_64/ld-ng/syscall.o
build-x86_64/ld-ng/syscalls.o: libc/syscalls.c
	@mkdir -p build-x86_64/ld-ng
	@mkdir -p dep/ld-ng
	$(call MP_INFO,CC	syscalls.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/ld-ng/syscalls.d -c libc/syscalls.c -o build-x86_64/ld-ng/syscalls.o
build-x86_64/ld-ng/elf-ng.o: linker/elf-ng.c
	@mkdir -p build-x86_64/ld-ng
	@mkdir -p dep/ld-ng
	$(call MP_INFO,CC	elf-ng.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/ld-ng/elf-ng.d -c linker/elf-ng.c -o build-x86_64/ld-ng/elf-ng.o
build-x86_64/ld-ng/ldso.o: linker/ldso.c
	@mkdir -p build-x86_64/ld-ng
	@mkdir -p dep/ld-ng
	$(call MP_INFO,CC	ldso.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/ld-ng/ldso.d -c linker/ldso.c -o build-x86_64/ld-ng/ldso.o
build-x86_64/ld-ng/pltstub.o: linker/pltstub.S
	@mkdir -p build-x86_64/ld-ng
	@mkdir -p dep/ld-ng
	$(call MP_INFO,AS	pltstub.o)
	@gcc  -MD -MF dep/ld-ng/pltstub.d -c linker/pltstub.S -o build-x86_64/ld-ng/pltstub.o
build-x86_64/testmod.ko: build-x86_64/modules/testmod.o
	$(call MP_INFO,CP	testmod.ko)
	@cp --preserve=timestamps build-x86_64/modules/testmod.o build-x86_64/testmod.ko
build-x86_64/modules/testmod.o: modules/testmod.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	$(call MP_INFO,CC	testmod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/modules/testmod.d -c modules/testmod.c -o build-x86_64/modules/testmod.o
build-x86_64/syscall.ko: build-x86_64/modules/syscall.o
	$(call MP_INFO,CP	syscall.ko)
	@cp --preserve=timestamps build-x86_64/modules/syscall.o build-x86_64/syscall.ko
build-x86_64/modules/syscall.o: modules/syscall.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	$(call MP_INFO,CC	syscall.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/modules/syscall.d -c modules/syscall.c -o build-x86_64/modules/syscall.o
build-x86_64/crash.ko: build-x86_64/modules/crash.o
	$(call MP_INFO,CP	crash.ko)
	@cp --preserve=timestamps build-x86_64/modules/crash.o build-x86_64/crash.ko
build-x86_64/modules/crash.o: modules/crash.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	$(call MP_INFO,CC	crash.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/modules/crash.d -c modules/crash.c -o build-x86_64/modules/crash.o
build-x86_64/bss.ko: build-x86_64/modules/bss.o
	$(call MP_INFO,CP	bss.ko)
	@cp --preserve=timestamps build-x86_64/modules/bss.o build-x86_64/bss.ko
build-x86_64/modules/bss.o: modules/bss.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	$(call MP_INFO,CC	bss.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/modules/bss.d -c modules/bss.c -o build-x86_64/modules/bss.o
build-x86_64/file.ko: build-x86_64/modules/file.o
	$(call MP_INFO,CP	file.ko)
	@cp --preserve=timestamps build-x86_64/modules/file.o build-x86_64/file.ko
build-x86_64/modules/file.o: modules/file.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	$(call MP_INFO,CC	file.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/modules/file.d -c modules/file.c -o build-x86_64/modules/file.o
build-x86_64/procmod.ko: build-x86_64/modules/procmod.o
	$(call MP_INFO,CP	procmod.ko)
	@cp --preserve=timestamps build-x86_64/modules/procmod.o build-x86_64/procmod.ko
build-x86_64/modules/procmod.o: modules/procmod.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	$(call MP_INFO,CC	procmod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/modules/procmod.d -c modules/procmod.c -o build-x86_64/modules/procmod.o
build-x86_64/thread.ko: build-x86_64/modules/thread.o
	$(call MP_INFO,CP	thread.ko)
	@cp --preserve=timestamps build-x86_64/modules/thread.o build-x86_64/thread.ko
build-x86_64/modules/thread.o: modules/thread.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	$(call MP_INFO,CC	thread.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -pedantic -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -fsanitize=undefined $(KERNEL_CFLAGS) -MD -MF dep/modules/thread.d -c modules/thread.c -o build-x86_64/modules/thread.o
build-x86_64/args: build-x86_64/user/args.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	args)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/args build-x86_64/user/args.o 
build-x86_64/user/args.o: user/args.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	args.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/args.d -c user/args.c -o build-x86_64/user/args.o
build-x86_64/bg: build-x86_64/user/bg.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	bg)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/bg build-x86_64/user/bg.o 
build-x86_64/user/bg.o: user/bg.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	bg.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/bg.d -c user/bg.c -o build-x86_64/user/bg.o
build-x86_64/bomb: build-x86_64/user/bomb.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	bomb)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/bomb build-x86_64/user/bomb.o 
build-x86_64/user/bomb.o: user/bomb.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	bomb.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/bomb.d -c user/bomb.c -o build-x86_64/user/bomb.o
build-x86_64/create: build-x86_64/user/create.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	create)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/create build-x86_64/user/create.o 
build-x86_64/user/create.o: user/create.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	create.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/create.d -c user/create.c -o build-x86_64/user/create.o
build-x86_64/echo: build-x86_64/user/echo.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	echo)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/echo build-x86_64/user/echo.o 
build-x86_64/user/echo.o: user/echo.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	echo.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/echo.d -c user/echo.c -o build-x86_64/user/echo.o
build-x86_64/false: build-x86_64/user/false.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	false)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/false build-x86_64/user/false.o 
build-x86_64/user/false.o: user/false.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	false.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/false.d -c user/false.c -o build-x86_64/user/false.o
build-x86_64/float: build-x86_64/user/float.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	float)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/float build-x86_64/user/float.o 
build-x86_64/user/float.o: user/float.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	float.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/float.d -c user/float.c -o build-x86_64/user/float.o
build-x86_64/hog: build-x86_64/user/hog.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	hog)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/hog build-x86_64/user/hog.o 
build-x86_64/user/hog.o: user/hog.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	hog.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/hog.d -c user/hog.c -o build-x86_64/user/hog.o
build-x86_64/modsys: build-x86_64/user/modsys.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	modsys)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/modsys build-x86_64/user/modsys.o 
build-x86_64/user/modsys.o: user/modsys.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	modsys.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/modsys.d -c user/modsys.c -o build-x86_64/user/modsys.o
build-x86_64/oom: build-x86_64/user/oom.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	oom)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/oom build-x86_64/user/oom.o 
build-x86_64/user/oom.o: user/oom.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	oom.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/oom.d -c user/oom.c -o build-x86_64/user/oom.o
build-x86_64/sleep: build-x86_64/user/sleep.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	sleep)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/sleep build-x86_64/user/sleep.o 
build-x86_64/user/sleep.o: user/sleep.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	sleep.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/sleep.d -c user/sleep.c -o build-x86_64/user/sleep.o
build-x86_64/time: build-x86_64/user/time.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	time)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/time build-x86_64/user/time.o 
build-x86_64/user/time.o: user/time.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	time.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/time.d -c user/time.c -o build-x86_64/user/time.o
build-x86_64/top: build-x86_64/user/top.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	top)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/top build-x86_64/user/top.o 
build-x86_64/user/top.o: user/top.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	top.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/top.d -c user/top.c -o build-x86_64/user/top.o
build-x86_64/trace: build-x86_64/user/trace.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	trace)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/trace build-x86_64/user/trace.o 
build-x86_64/user/trace.o: user/trace.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	trace.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/trace.d -c user/trace.c -o build-x86_64/user/trace.o
build-x86_64/chmod: build-x86_64/user/chmod.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	chmod)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/chmod build-x86_64/user/chmod.o 
build-x86_64/user/chmod.o: user/chmod.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	chmod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/chmod.d -c user/chmod.c -o build-x86_64/user/chmod.o
build-x86_64/mmap: build-x86_64/user/mmap.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	mmap)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/mmap build-x86_64/user/mmap.o 
build-x86_64/user/mmap.o: user/mmap.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	mmap.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/mmap.d -c user/mmap.c -o build-x86_64/user/mmap.o
build-x86_64/rm: build-x86_64/user/rm.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	rm)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/rm build-x86_64/user/rm.o 
build-x86_64/user/rm.o: user/rm.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	rm.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/rm.d -c user/rm.c -o build-x86_64/user/rm.o
build-x86_64/ab: build-x86_64/user/ab.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	ab)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/ab build-x86_64/user/ab.o 
build-x86_64/user/ab.o: user/ab.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	ab.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/ab.d -c user/ab.c -o build-x86_64/user/ab.o
build-x86_64/bf: build-x86_64/user/bf.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	bf)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/bf build-x86_64/user/bf.o 
build-x86_64/user/bf.o: user/bf.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	bf.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/bf.d -c user/bf.c -o build-x86_64/user/bf.o
build-x86_64/bf2: build-x86_64/user/bf2.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	bf2)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/bf2 build-x86_64/user/bf2.o 
build-x86_64/user/bf2.o: user/bf2.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	bf2.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/bf2.d -c user/bf2.c -o build-x86_64/user/bf2.o
build-x86_64/busy: build-x86_64/user/busy.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	busy)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/busy build-x86_64/user/busy.o 
build-x86_64/user/busy.o: user/busy.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	busy.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/busy.d -c user/busy.c -o build-x86_64/user/busy.o
build-x86_64/cat: build-x86_64/user/cat.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	cat)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/cat build-x86_64/user/cat.o 
build-x86_64/user/cat.o: user/cat.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	cat.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/cat.d -c user/cat.c -o build-x86_64/user/cat.o
build-x86_64/column: build-x86_64/user/column.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	column)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/column build-x86_64/user/column.o 
build-x86_64/user/column.o: user/column.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	column.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/column.d -c user/column.c -o build-x86_64/user/column.o
build-x86_64/crash: build-x86_64/user/crash.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	crash)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/crash build-x86_64/user/crash.o 
build-x86_64/user/crash.o: user/crash.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	crash.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/crash.d -c user/crash.c -o build-x86_64/user/crash.o
build-x86_64/forks: build-x86_64/user/forks.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	forks)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/forks build-x86_64/user/forks.o 
build-x86_64/user/forks.o: user/forks.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	forks.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/forks.d -c user/forks.c -o build-x86_64/user/forks.o
build-x86_64/head: build-x86_64/user/head.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	head)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/head build-x86_64/user/head.o 
build-x86_64/user/head.o: user/head.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	head.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/head.d -c user/head.c -o build-x86_64/user/head.o
build-x86_64/kill: build-x86_64/user/kill.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	kill)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/kill build-x86_64/user/kill.o 
build-x86_64/user/kill.o: user/kill.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	kill.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/kill.d -c user/kill.c -o build-x86_64/user/kill.o
build-x86_64/rot13: build-x86_64/user/rot13.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	rot13)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/rot13 build-x86_64/user/rot13.o 
build-x86_64/user/rot13.o: user/rot13.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	rot13.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/rot13.d -c user/rot13.c -o build-x86_64/user/rot13.o
build-x86_64/uname: build-x86_64/user/uname.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	uname)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/uname build-x86_64/user/uname.o 
build-x86_64/user/uname.o: user/uname.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	uname.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/uname.d -c user/uname.c -o build-x86_64/user/uname.o
build-x86_64/uthread: build-x86_64/user/uthread.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	uthread)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/uthread build-x86_64/user/uthread.o 
build-x86_64/user/uthread.o: user/uthread.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	uthread.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/uthread.d -c user/uthread.c -o build-x86_64/user/uthread.o
build-x86_64/xd: build-x86_64/user/xd.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	xd)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/xd build-x86_64/user/xd.o 
build-x86_64/user/xd.o: user/xd.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	xd.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/xd.d -c user/xd.c -o build-x86_64/user/xd.o
build-x86_64/dg: build-x86_64/user/dg.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	dg)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/dg build-x86_64/user/dg.o 
build-x86_64/user/dg.o: user/dg.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	dg.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/dg.d -c user/dg.c -o build-x86_64/user/dg.o
build-x86_64/sc: build-x86_64/user/sc.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	sc)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/sc build-x86_64/user/sc.o 
build-x86_64/user/sc.o: user/sc.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	sc.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/sc.d -c user/sc.c -o build-x86_64/user/sc.o
build-x86_64/st: build-x86_64/user/st.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	st)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/st build-x86_64/user/st.o 
build-x86_64/user/st.o: user/st.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	st.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/st.d -c user/st.c -o build-x86_64/user/st.o
build-x86_64/threads: build-x86_64/user/threads.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	threads)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/threads build-x86_64/user/threads.o 
build-x86_64/user/threads.o: user/threads.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	threads.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/threads.d -c user/threads.c -o build-x86_64/user/threads.o
build-x86_64/test: build-x86_64/user/test.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	test)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/test build-x86_64/user/test.o 
build-x86_64/user/test.o: user/test.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	test.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/test.d -c user/test.c -o build-x86_64/user/test.o
build-x86_64/clone: build-x86_64/user/clone.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	clone)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/clone build-x86_64/user/clone.o 
build-x86_64/user/clone.o: user/clone.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	clone.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/clone.d -c user/clone.c -o build-x86_64/user/clone.o
build-x86_64/ls: build-x86_64/user/ls.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	ls)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/ls build-x86_64/user/ls.o 
build-x86_64/user/ls.o: user/ls.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	ls.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/ls.d -c user/ls.c -o build-x86_64/user/ls.o
build-x86_64/step: build-x86_64/user/step.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	step)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/step build-x86_64/user/step.o -lelf
build-x86_64/user/step.o: user/step.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	step.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/step.d -c user/step.c -o build-x86_64/user/step.o
build-x86_64/traceback: build-x86_64/user/traceback.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	traceback)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/traceback build-x86_64/user/traceback.o 
build-x86_64/user/traceback.o: user/traceback.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	traceback.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/traceback.d -c user/traceback.c -o build-x86_64/user/traceback.o
build-x86_64/insmod: build-x86_64/user/insmod.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	insmod)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/insmod build-x86_64/user/insmod.o 
build-x86_64/user/insmod.o: user/insmod.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	insmod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/insmod.d -c user/insmod.c -o build-x86_64/user/insmod.o
build-x86_64/cforks: build-x86_64/user/cforks.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	cforks)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/cforks build-x86_64/user/cforks.o 
build-x86_64/user/cforks.o: user/cforks.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	cforks.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/cforks.d -c user/cforks.c -o build-x86_64/user/cforks.o
build-x86_64/strace: build-x86_64/user/strace.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	strace)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/strace build-x86_64/user/strace.o 
build-x86_64/user/strace.o: user/strace.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	strace.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/strace.d -c user/strace.c -o build-x86_64/user/strace.o
build-x86_64/init: build-x86_64/user/init.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	init)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/init build-x86_64/user/init.o 
build-x86_64/user/init.o: user/init.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	init.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/init.d -c user/init.c -o build-x86_64/user/init.o
build-x86_64/mb: build-x86_64/user/mb.o sysroot/usr/lib/libc.so sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a
	$(call MP_INFO,LD	mb)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/mb build-x86_64/user/mb.o -lm
build-x86_64/user/mb.o: user/mb.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(call MP_INFO,CC	mb.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -g -O2 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member $(CFLAGS) -Wno-builtin-declaration-mismatch $(USER_CFLAGS) -static -MD -MF dep/user/mb.d -c user/mb.c -o build-x86_64/user/mb.o

sysroot/usr/lib/libc.a: build-x86_64/libc.a
	$(call MP_INFO,install	libc.a)
	@cp build-x86_64/libc.a sysroot/usr/lib/libc.a

sysroot/usr/lib/libm.a: build-x86_64/libm.a
	$(call MP_INFO,install	libm.a)
	@cp build-x86_64/libm.a sysroot/usr/lib/libm.a

sysroot/usr/lib/libelf.a: build-x86_64/libelf.a
	$(call MP_INFO,install	libelf.a)
	@cp build-x86_64/libelf.a sysroot/usr/lib/libelf.a

sysroot/usr/lib/libc.so: build-x86_64/libc.so
	$(call MP_INFO,install	libc.so)
	@cp build-x86_64/libc.so sysroot/usr/lib/libc.so

ngk.elf: build-x86_64/ngk.elf
	$(call MP_INFO,install	ngk.elf)
	@cp build-x86_64/ngk.elf ngk.elf

sysroot/usr/lib/crt0.o: build-x86_64/crt0.o
	$(call MP_INFO,install	crt0.o)
	@cp build-x86_64/crt0.o sysroot/usr/lib/crt0.o

sysroot/bin/sh: build-x86_64/sh
	$(call MP_INFO,install	sh)
	@cp build-x86_64/sh sysroot/bin/sh

sysroot/bin/ld-ng.so: build-x86_64/ld-ng.so
	$(call MP_INFO,install	ld-ng.so)
	@cp build-x86_64/ld-ng.so sysroot/bin/ld-ng.so

sysroot/bin/testmod.ko: build-x86_64/testmod.ko
	$(call MP_INFO,install	testmod.ko)
	@cp build-x86_64/testmod.ko sysroot/bin/testmod.ko

sysroot/bin/syscall.ko: build-x86_64/syscall.ko
	$(call MP_INFO,install	syscall.ko)
	@cp build-x86_64/syscall.ko sysroot/bin/syscall.ko

sysroot/bin/crash.ko: build-x86_64/crash.ko
	$(call MP_INFO,install	crash.ko)
	@cp build-x86_64/crash.ko sysroot/bin/crash.ko

sysroot/bin/bss.ko: build-x86_64/bss.ko
	$(call MP_INFO,install	bss.ko)
	@cp build-x86_64/bss.ko sysroot/bin/bss.ko

sysroot/bin/file.ko: build-x86_64/file.ko
	$(call MP_INFO,install	file.ko)
	@cp build-x86_64/file.ko sysroot/bin/file.ko

sysroot/bin/procmod.ko: build-x86_64/procmod.ko
	$(call MP_INFO,install	procmod.ko)
	@cp build-x86_64/procmod.ko sysroot/bin/procmod.ko

sysroot/bin/thread.ko: build-x86_64/thread.ko
	$(call MP_INFO,install	thread.ko)
	@cp build-x86_64/thread.ko sysroot/bin/thread.ko

sysroot/bin/args: build-x86_64/args
	$(call MP_INFO,install	args)
	@cp build-x86_64/args sysroot/bin/args

sysroot/bin/bg: build-x86_64/bg
	$(call MP_INFO,install	bg)
	@cp build-x86_64/bg sysroot/bin/bg

sysroot/bin/bomb: build-x86_64/bomb
	$(call MP_INFO,install	bomb)
	@cp build-x86_64/bomb sysroot/bin/bomb

sysroot/bin/create: build-x86_64/create
	$(call MP_INFO,install	create)
	@cp build-x86_64/create sysroot/bin/create

sysroot/bin/echo: build-x86_64/echo
	$(call MP_INFO,install	echo)
	@cp build-x86_64/echo sysroot/bin/echo

sysroot/bin/false: build-x86_64/false
	$(call MP_INFO,install	false)
	@cp build-x86_64/false sysroot/bin/false

sysroot/bin/float: build-x86_64/float
	$(call MP_INFO,install	float)
	@cp build-x86_64/float sysroot/bin/float

sysroot/bin/hog: build-x86_64/hog
	$(call MP_INFO,install	hog)
	@cp build-x86_64/hog sysroot/bin/hog

sysroot/bin/modsys: build-x86_64/modsys
	$(call MP_INFO,install	modsys)
	@cp build-x86_64/modsys sysroot/bin/modsys

sysroot/bin/oom: build-x86_64/oom
	$(call MP_INFO,install	oom)
	@cp build-x86_64/oom sysroot/bin/oom

sysroot/bin/sleep: build-x86_64/sleep
	$(call MP_INFO,install	sleep)
	@cp build-x86_64/sleep sysroot/bin/sleep

sysroot/bin/time: build-x86_64/time
	$(call MP_INFO,install	time)
	@cp build-x86_64/time sysroot/bin/time

sysroot/bin/top: build-x86_64/top
	$(call MP_INFO,install	top)
	@cp build-x86_64/top sysroot/bin/top

sysroot/bin/trace: build-x86_64/trace
	$(call MP_INFO,install	trace)
	@cp build-x86_64/trace sysroot/bin/trace

sysroot/bin/chmod: build-x86_64/chmod
	$(call MP_INFO,install	chmod)
	@cp build-x86_64/chmod sysroot/bin/chmod

sysroot/bin/mmap: build-x86_64/mmap
	$(call MP_INFO,install	mmap)
	@cp build-x86_64/mmap sysroot/bin/mmap

sysroot/bin/rm: build-x86_64/rm
	$(call MP_INFO,install	rm)
	@cp build-x86_64/rm sysroot/bin/rm

sysroot/bin/ab: build-x86_64/ab
	$(call MP_INFO,install	ab)
	@cp build-x86_64/ab sysroot/bin/ab

sysroot/bin/bf: build-x86_64/bf
	$(call MP_INFO,install	bf)
	@cp build-x86_64/bf sysroot/bin/bf

sysroot/bin/bf2: build-x86_64/bf2
	$(call MP_INFO,install	bf2)
	@cp build-x86_64/bf2 sysroot/bin/bf2

sysroot/bin/busy: build-x86_64/busy
	$(call MP_INFO,install	busy)
	@cp build-x86_64/busy sysroot/bin/busy

sysroot/bin/cat: build-x86_64/cat
	$(call MP_INFO,install	cat)
	@cp build-x86_64/cat sysroot/bin/cat

sysroot/bin/column: build-x86_64/column
	$(call MP_INFO,install	column)
	@cp build-x86_64/column sysroot/bin/column

sysroot/bin/crash: build-x86_64/crash
	$(call MP_INFO,install	crash)
	@cp build-x86_64/crash sysroot/bin/crash

sysroot/bin/forks: build-x86_64/forks
	$(call MP_INFO,install	forks)
	@cp build-x86_64/forks sysroot/bin/forks

sysroot/bin/head: build-x86_64/head
	$(call MP_INFO,install	head)
	@cp build-x86_64/head sysroot/bin/head

sysroot/bin/kill: build-x86_64/kill
	$(call MP_INFO,install	kill)
	@cp build-x86_64/kill sysroot/bin/kill

sysroot/bin/rot13: build-x86_64/rot13
	$(call MP_INFO,install	rot13)
	@cp build-x86_64/rot13 sysroot/bin/rot13

sysroot/bin/uname: build-x86_64/uname
	$(call MP_INFO,install	uname)
	@cp build-x86_64/uname sysroot/bin/uname

sysroot/bin/uthread: build-x86_64/uthread
	$(call MP_INFO,install	uthread)
	@cp build-x86_64/uthread sysroot/bin/uthread

sysroot/bin/xd: build-x86_64/xd
	$(call MP_INFO,install	xd)
	@cp build-x86_64/xd sysroot/bin/xd

sysroot/bin/dg: build-x86_64/dg
	$(call MP_INFO,install	dg)
	@cp build-x86_64/dg sysroot/bin/dg

sysroot/bin/sc: build-x86_64/sc
	$(call MP_INFO,install	sc)
	@cp build-x86_64/sc sysroot/bin/sc

sysroot/bin/st: build-x86_64/st
	$(call MP_INFO,install	st)
	@cp build-x86_64/st sysroot/bin/st

sysroot/bin/threads: build-x86_64/threads
	$(call MP_INFO,install	threads)
	@cp build-x86_64/threads sysroot/bin/threads

sysroot/bin/test: build-x86_64/test
	$(call MP_INFO,install	test)
	@cp build-x86_64/test sysroot/bin/test

sysroot/bin/clone: build-x86_64/clone
	$(call MP_INFO,install	clone)
	@cp build-x86_64/clone sysroot/bin/clone

sysroot/bin/ls: build-x86_64/ls
	$(call MP_INFO,install	ls)
	@cp build-x86_64/ls sysroot/bin/ls

sysroot/bin/step: build-x86_64/step
	$(call MP_INFO,install	step)
	@cp build-x86_64/step sysroot/bin/step

sysroot/bin/traceback: build-x86_64/traceback
	$(call MP_INFO,install	traceback)
	@cp build-x86_64/traceback sysroot/bin/traceback

sysroot/bin/insmod: build-x86_64/insmod
	$(call MP_INFO,install	insmod)
	@cp build-x86_64/insmod sysroot/bin/insmod

sysroot/bin/cforks: build-x86_64/cforks
	$(call MP_INFO,install	cforks)
	@cp build-x86_64/cforks sysroot/bin/cforks

sysroot/bin/strace: build-x86_64/strace
	$(call MP_INFO,install	strace)
	@cp build-x86_64/strace sysroot/bin/strace

sysroot/bin/init: build-x86_64/init
	$(call MP_INFO,install	init)
	@cp build-x86_64/init sysroot/bin/init

sysroot/bin/mb: build-x86_64/mb
	$(call MP_INFO,install	mb)
	@cp build-x86_64/mb sysroot/bin/mb


mp_install: $(MP_ALL_INSTALL)

mp_clean:
	@echo "Clean magpie objects"
	@rm -f $(MP_ALL_OBJECTS)


MP_ALL_INSTALL_TARGETS := sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a sysroot/usr/lib/libc.so ngk.elf sysroot/usr/lib/crt0.o sysroot/bin/sh sysroot/bin/ld-ng.so sysroot/bin/testmod.ko sysroot/bin/syscall.ko sysroot/bin/crash.ko sysroot/bin/bss.ko sysroot/bin/file.ko sysroot/bin/procmod.ko sysroot/bin/thread.ko sysroot/bin/args sysroot/bin/bg sysroot/bin/bomb sysroot/bin/create sysroot/bin/echo sysroot/bin/false sysroot/bin/float sysroot/bin/hog sysroot/bin/modsys sysroot/bin/oom sysroot/bin/sleep sysroot/bin/time sysroot/bin/top sysroot/bin/trace sysroot/bin/chmod sysroot/bin/mmap sysroot/bin/rm sysroot/bin/ab sysroot/bin/bf sysroot/bin/bf2 sysroot/bin/busy sysroot/bin/cat sysroot/bin/column sysroot/bin/crash sysroot/bin/forks sysroot/bin/head sysroot/bin/kill sysroot/bin/rot13 sysroot/bin/uname sysroot/bin/uthread sysroot/bin/xd sysroot/bin/dg sysroot/bin/sc sysroot/bin/st sysroot/bin/threads sysroot/bin/test sysroot/bin/clone sysroot/bin/ls sysroot/bin/step sysroot/bin/traceback sysroot/bin/insmod sysroot/bin/cforks sysroot/bin/strace sysroot/bin/init sysroot/bin/mb
MP_ALL_INSTALL := sysroot/usr/lib/libc.a sysroot/usr/lib/libm.a sysroot/usr/lib/libelf.a sysroot/usr/lib/libc.so build-x86_64/libk.a ngk.elf sysroot/usr/lib/crt0.o sysroot/bin/sh sysroot/bin/ld-ng.so sysroot/bin/testmod.ko sysroot/bin/syscall.ko sysroot/bin/crash.ko sysroot/bin/bss.ko sysroot/bin/file.ko sysroot/bin/procmod.ko sysroot/bin/thread.ko sysroot/bin/args sysroot/bin/bg sysroot/bin/bomb sysroot/bin/create sysroot/bin/echo sysroot/bin/false sysroot/bin/float sysroot/bin/hog sysroot/bin/modsys sysroot/bin/oom sysroot/bin/sleep sysroot/bin/time sysroot/bin/top sysroot/bin/trace sysroot/bin/chmod sysroot/bin/mmap sysroot/bin/rm sysroot/bin/ab sysroot/bin/bf sysroot/bin/bf2 sysroot/bin/busy sysroot/bin/cat sysroot/bin/column sysroot/bin/crash sysroot/bin/forks sysroot/bin/head sysroot/bin/kill sysroot/bin/rot13 sysroot/bin/uname sysroot/bin/uthread sysroot/bin/xd sysroot/bin/dg sysroot/bin/sc sysroot/bin/st sysroot/bin/threads sysroot/bin/test sysroot/bin/clone sysroot/bin/ls sysroot/bin/step sysroot/bin/traceback sysroot/bin/insmod sysroot/bin/cforks sysroot/bin/strace sysroot/bin/init sysroot/bin/mb
MP_ALL_TARGETS := build-x86_64/libc.a build-x86_64/libm.a build-x86_64/libelf.a build-x86_64/libc.so build-x86_64/libk.a build-x86_64/ngk.elf build-x86_64/crt0.o build-x86_64/sh build-x86_64/ld-ng.so build-x86_64/testmod.ko build-x86_64/syscall.ko build-x86_64/crash.ko build-x86_64/bss.ko build-x86_64/file.ko build-x86_64/procmod.ko build-x86_64/thread.ko build-x86_64/args build-x86_64/bg build-x86_64/bomb build-x86_64/create build-x86_64/echo build-x86_64/false build-x86_64/float build-x86_64/hog build-x86_64/modsys build-x86_64/oom build-x86_64/sleep build-x86_64/time build-x86_64/top build-x86_64/trace build-x86_64/chmod build-x86_64/mmap build-x86_64/rm build-x86_64/ab build-x86_64/bf build-x86_64/bf2 build-x86_64/busy build-x86_64/cat build-x86_64/column build-x86_64/crash build-x86_64/forks build-x86_64/head build-x86_64/kill build-x86_64/rot13 build-x86_64/uname build-x86_64/uthread build-x86_64/xd build-x86_64/dg build-x86_64/sc build-x86_64/st build-x86_64/threads build-x86_64/test build-x86_64/clone build-x86_64/ls build-x86_64/step build-x86_64/traceback build-x86_64/insmod build-x86_64/cforks build-x86_64/strace build-x86_64/init build-x86_64/mb
MP_ALL_OBJECTS := build-x86_64/libc.a build-x86_64/libc/crt0.o build-x86_64/libc/crti.o build-x86_64/libc/crtn.o build-x86_64/libc/ctype.o build-x86_64/libc/entry.o build-x86_64/libc/errno.o build-x86_64/libc/fstdio.o build-x86_64/libc/fstdio_unlocked.o build-x86_64/libc/getopt.o build-x86_64/libc/locale.o build-x86_64/libc/malloc.o build-x86_64/libc/nightingale.o build-x86_64/libc/setjmp.o build-x86_64/libc/signal.o build-x86_64/libc/stdio.o build-x86_64/libc/stdlib.o build-x86_64/libc/string.o build-x86_64/libc/strtod.o build-x86_64/libc/syscall.o build-x86_64/libc/syscalls.o build-x86_64/libc/time.o build-x86_64/libc/todo.o build-x86_64/libc/unistd.o build-x86_64/libc/vector.o build-x86_64/libc/x86_64/nightingale.o build-x86_64/libm.a build-x86_64/libm/complex.o build-x86_64/libm/double.o build-x86_64/libelf.a build-x86_64/libelf/elf-ng.o build-x86_64/libc.so build-x86_64/libc_so/crt0.o build-x86_64/libc_so/crti.o build-x86_64/libc_so/crtn.o build-x86_64/libc_so/ctype.o build-x86_64/libc_so/entry.o build-x86_64/libc_so/errno.o build-x86_64/libc_so/fstdio.o build-x86_64/libc_so/fstdio_unlocked.o build-x86_64/libc_so/getopt.o build-x86_64/libc_so/locale.o build-x86_64/libc_so/malloc.o build-x86_64/libc_so/nightingale.o build-x86_64/libc_so/setjmp.o build-x86_64/libc_so/signal.o build-x86_64/libc_so/stdio.o build-x86_64/libc_so/stdlib.o build-x86_64/libc_so/string.o build-x86_64/libc_so/strtod.o build-x86_64/libc_so/syscall.o build-x86_64/libc_so/syscalls.o build-x86_64/libc_so/time.o build-x86_64/libc_so/todo.o build-x86_64/libc_so/unistd.o build-x86_64/libc_so/vector.o build-x86_64/libc_so/x86_64/nightingale.o build-x86_64/libk.a build-x86_64/libk/ctype.o build-x86_64/libk/errno.o build-x86_64/libk/malloc.o build-x86_64/libk/setjmp.o build-x86_64/libk/signal.o build-x86_64/libk/stdio.o build-x86_64/libk/stdlib.o build-x86_64/libk/string.o build-x86_64/libk/x86_64/nightingale.o build-x86_64/ngk.elf build-x86_64/fs/char_devices.o build-x86_64/fs/directory.o build-x86_64/fs/fs.o build-x86_64/fs/membuf.o build-x86_64/fs/pipe.o build-x86_64/fs/procfs.o build-x86_64/fs/socket.o build-x86_64/kernel/debug.o build-x86_64/kernel/dmgr.o build-x86_64/kernel/elf.o build-x86_64/kernel/exec.o build-x86_64/kernel/irq.o build-x86_64/kernel/main.o build-x86_64/kernel/mman.o build-x86_64/kernel/mod.o build-x86_64/kernel/multiboot.o build-x86_64/kernel/pci.o build-x86_64/kernel/pmm.o build-x86_64/kernel/rand.o build-x86_64/kernel/ringbuf.o build-x86_64/kernel/serial.o build-x86_64/kernel/signal.o build-x86_64/kernel/spalloc.o build-x86_64/kernel/string.o build-x86_64/kernel/sync.o build-x86_64/kernel/sync_testbed.o build-x86_64/kernel/syscall.o build-x86_64/kernel/tarfs.o build-x86_64/kernel/tests.o build-x86_64/kernel/thread.o build-x86_64/kernel/timer.o build-x86_64/kernel/trace.o build-x86_64/kernel/tty.o build-x86_64/kernel/ubsan.o build-x86_64/kernel/uname.o build-x86_64/linker/elf-ng.o build-x86_64/linker/modld.o build-x86_64/x86/acpi.o build-x86_64/x86/apic.o build-x86_64/x86/boot.o build-x86_64/x86/cpu.o build-x86_64/x86/halt.o build-x86_64/x86/interrupt.o build-x86_64/x86/isrs.o build-x86_64/x86/pic.o build-x86_64/x86/pit.o build-x86_64/x86/uart.o build-x86_64/x86/vmm.o build-x86_64/crt0.o build-x86_64/crt/crt0.o build-x86_64/sh build-x86_64/sh-/eval.o build-x86_64/sh-/parse.o build-x86_64/sh-/readline.o build-x86_64/sh-/sh.o build-x86_64/sh-/token.o build-x86_64/ld-ng.so build-x86_64/ld-ng/syscall.o build-x86_64/ld-ng/syscalls.o build-x86_64/ld-ng/elf-ng.o build-x86_64/ld-ng/ldso.o build-x86_64/ld-ng/pltstub.o build-x86_64/testmod.ko build-x86_64/modules/testmod.o build-x86_64/syscall.ko build-x86_64/modules/syscall.o build-x86_64/crash.ko build-x86_64/modules/crash.o build-x86_64/bss.ko build-x86_64/modules/bss.o build-x86_64/file.ko build-x86_64/modules/file.o build-x86_64/procmod.ko build-x86_64/modules/procmod.o build-x86_64/thread.ko build-x86_64/modules/thread.o build-x86_64/args build-x86_64/user/args.o build-x86_64/bg build-x86_64/user/bg.o build-x86_64/bomb build-x86_64/user/bomb.o build-x86_64/create build-x86_64/user/create.o build-x86_64/echo build-x86_64/user/echo.o build-x86_64/false build-x86_64/user/false.o build-x86_64/float build-x86_64/user/float.o build-x86_64/hog build-x86_64/user/hog.o build-x86_64/modsys build-x86_64/user/modsys.o build-x86_64/oom build-x86_64/user/oom.o build-x86_64/sleep build-x86_64/user/sleep.o build-x86_64/time build-x86_64/user/time.o build-x86_64/top build-x86_64/user/top.o build-x86_64/trace build-x86_64/user/trace.o build-x86_64/chmod build-x86_64/user/chmod.o build-x86_64/mmap build-x86_64/user/mmap.o build-x86_64/rm build-x86_64/user/rm.o build-x86_64/ab build-x86_64/user/ab.o build-x86_64/bf build-x86_64/user/bf.o build-x86_64/bf2 build-x86_64/user/bf2.o build-x86_64/busy build-x86_64/user/busy.o build-x86_64/cat build-x86_64/user/cat.o build-x86_64/column build-x86_64/user/column.o build-x86_64/crash build-x86_64/user/crash.o build-x86_64/forks build-x86_64/user/forks.o build-x86_64/head build-x86_64/user/head.o build-x86_64/kill build-x86_64/user/kill.o build-x86_64/rot13 build-x86_64/user/rot13.o build-x86_64/uname build-x86_64/user/uname.o build-x86_64/uthread build-x86_64/user/uthread.o build-x86_64/xd build-x86_64/user/xd.o build-x86_64/dg build-x86_64/user/dg.o build-x86_64/sc build-x86_64/user/sc.o build-x86_64/st build-x86_64/user/st.o build-x86_64/threads build-x86_64/user/threads.o build-x86_64/test build-x86_64/user/test.o build-x86_64/clone build-x86_64/user/clone.o build-x86_64/ls build-x86_64/user/ls.o build-x86_64/step build-x86_64/user/step.o build-x86_64/traceback build-x86_64/user/traceback.o build-x86_64/insmod build-x86_64/user/insmod.o build-x86_64/cforks build-x86_64/user/cforks.o build-x86_64/strace build-x86_64/user/strace.o build-x86_64/init build-x86_64/user/init.o build-x86_64/mb build-x86_64/user/mb.o

include $(shell find dep -name '*.d')
