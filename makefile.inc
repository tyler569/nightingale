# vim: ft=make :
.PHONY: mp_all mp_clean mp_install

mp_all: build-x86_64/libc.a build-x86_64/libc.so build-x86_64/libk.a build-x86_64/ngk.elf build-x86_64/crt0.o build-x86_64/sh build-x86_64/ld-ng.so build-x86_64/testmod.ko build-x86_64/thread.ko build-x86_64/syscall.ko build-x86_64/threads build-x86_64/hog build-x86_64/echo build-x86_64/oom build-x86_64/args build-x86_64/ls build-x86_64/modsys build-x86_64/fio build-x86_64/xd build-x86_64/busy build-x86_64/uthread build-x86_64/false build-x86_64/crash build-x86_64/sleep build-x86_64/fcat build-x86_64/time build-x86_64/uname build-x86_64/kill build-x86_64/syscall_test build-x86_64/ab build-x86_64/test build-x86_64/strace build-x86_64/forks build-x86_64/bg build-x86_64/float build-x86_64/clone build-x86_64/init build-x86_64/mmap build-x86_64/cat build-x86_64/bf build-x86_64/column build-x86_64/bomb build-x86_64/create build-x86_64/rot13 build-x86_64/trace build-x86_64/top build-x86_64/bf2 build-x86_64/insmod build-x86_64/head

mp_clean:
	@echo "Clean magpie objects"
	@rm -f build-x86_64/libc.a build-x86_64/libc/crt0.o build-x86_64/libc/crti.o build-x86_64/libc/crtn.o build-x86_64/libc/ctype.o build-x86_64/libc/entry.o build-x86_64/libc/errno.o build-x86_64/libc/fstdio.o build-x86_64/libc/fstdio_unlocked.o build-x86_64/libc/getopt.o build-x86_64/libc/locale.o build-x86_64/libc/malloc.o build-x86_64/libc/nightingale.o build-x86_64/libc/setjmp.o build-x86_64/libc/signal.o build-x86_64/libc/stdio.o build-x86_64/libc/stdlib.o build-x86_64/libc/string.o build-x86_64/libc/strtod.o build-x86_64/libc/syscall.o build-x86_64/libc/syscalls.o build-x86_64/libc/time.o build-x86_64/libc/todo.o build-x86_64/libc/unistd.o build-x86_64/libc/vector.o build-x86_64/libc/x86_64/nightingale.o build-x86_64/libc.so build-x86_64/libc_so/crt0.o build-x86_64/libc_so/crti.o build-x86_64/libc_so/crtn.o build-x86_64/libc_so/ctype.o build-x86_64/libc_so/entry.o build-x86_64/libc_so/errno.o build-x86_64/libc_so/fstdio.o build-x86_64/libc_so/fstdio_unlocked.o build-x86_64/libc_so/getopt.o build-x86_64/libc_so/locale.o build-x86_64/libc_so/malloc.o build-x86_64/libc_so/nightingale.o build-x86_64/libc_so/setjmp.o build-x86_64/libc_so/signal.o build-x86_64/libc_so/stdio.o build-x86_64/libc_so/stdlib.o build-x86_64/libc_so/string.o build-x86_64/libc_so/strtod.o build-x86_64/libc_so/syscall.o build-x86_64/libc_so/syscalls.o build-x86_64/libc_so/time.o build-x86_64/libc_so/todo.o build-x86_64/libc_so/unistd.o build-x86_64/libc_so/vector.o build-x86_64/libc_so/x86_64/nightingale.o build-x86_64/libk.a build-x86_64/libk/ctype.o build-x86_64/libk/errno.o build-x86_64/libk/malloc.o build-x86_64/libk/setjmp.o build-x86_64/libk/signal.o build-x86_64/libk/stdio.o build-x86_64/libk/stdlib.o build-x86_64/libk/string.o build-x86_64/libk/x86_64/nightingale.o build-x86_64/ngk.elf build-x86_64/fs/char_devices.o build-x86_64/fs/directory.o build-x86_64/fs/fs.o build-x86_64/fs/membuf.o build-x86_64/fs/pipe.o build-x86_64/fs/procfs.o build-x86_64/fs/socket.o build-x86_64/kernel/debug.o build-x86_64/kernel/dmgr.o build-x86_64/kernel/elf.o build-x86_64/kernel/exec.o build-x86_64/kernel/irq.o build-x86_64/kernel/main.o build-x86_64/kernel/mman.o build-x86_64/kernel/mod.o build-x86_64/kernel/multiboot.o build-x86_64/kernel/pci.o build-x86_64/kernel/pmm.o build-x86_64/kernel/rand.o build-x86_64/kernel/ringbuf.o build-x86_64/kernel/serial.o build-x86_64/kernel/signal.o build-x86_64/kernel/spalloc.o build-x86_64/kernel/string.o build-x86_64/kernel/sync.o build-x86_64/kernel/sync_testbed.o build-x86_64/kernel/syscall.o build-x86_64/kernel/tarfs.o build-x86_64/kernel/tests.o build-x86_64/kernel/thread.o build-x86_64/kernel/timer.o build-x86_64/kernel/trace.o build-x86_64/kernel/tty.o build-x86_64/kernel/ubsan.o build-x86_64/kernel/uname.o build-x86_64/linker/elf-ng.o build-x86_64/linker/modld.o build-x86_64/x86/acpi.o build-x86_64/x86/apic.o build-x86_64/x86/boot.o build-x86_64/x86/cpu.o build-x86_64/x86/halt.o build-x86_64/x86/interrupt.o build-x86_64/x86/isrs.o build-x86_64/x86/pic.o build-x86_64/x86/pit.o build-x86_64/x86/uart.o build-x86_64/x86/vmm.o build-x86_64/crt0.o build-x86_64/crt/crt0.o build-x86_64/sh build-x86_64/sh-/parse.o build-x86_64/sh-/readline.o build-x86_64/sh-/sh.o build-x86_64/sh-/token.o build-x86_64/ld-ng.so build-x86_64/ld-ng/elf-ng.o build-x86_64/ld-ng/ldso.o build-x86_64/testmod.ko build-x86_64/modules/testmod.o build-x86_64/thread.ko build-x86_64/modules/thread.o build-x86_64/syscall.ko build-x86_64/modules/syscall.o build-x86_64/threads build-x86_64/user/threads.o build-x86_64/hog build-x86_64/user/hog.o build-x86_64/echo build-x86_64/user/echo.o build-x86_64/oom build-x86_64/user/oom.o build-x86_64/args build-x86_64/user/args.o build-x86_64/ls build-x86_64/user/ls.o build-x86_64/modsys build-x86_64/user/modsys.o build-x86_64/fio build-x86_64/user/fio.o build-x86_64/xd build-x86_64/user/xd.o build-x86_64/busy build-x86_64/user/busy.o build-x86_64/uthread build-x86_64/user/uthread.o build-x86_64/false build-x86_64/user/false.o build-x86_64/crash build-x86_64/user/crash.o build-x86_64/sleep build-x86_64/user/sleep.o build-x86_64/fcat build-x86_64/user/fcat.o build-x86_64/time build-x86_64/user/time.o build-x86_64/uname build-x86_64/user/uname.o build-x86_64/kill build-x86_64/user/kill.o build-x86_64/syscall_test build-x86_64/user/syscall_test.o build-x86_64/ab build-x86_64/user/ab.o build-x86_64/test build-x86_64/user/test.o build-x86_64/strace build-x86_64/user/strace.o build-x86_64/forks build-x86_64/user/forks.o build-x86_64/bg build-x86_64/user/bg.o build-x86_64/float build-x86_64/user/float.o build-x86_64/clone build-x86_64/user/clone.o build-x86_64/init build-x86_64/user/init.o build-x86_64/mmap build-x86_64/user/mmap.o build-x86_64/cat build-x86_64/user/cat.o build-x86_64/bf build-x86_64/user/bf.o build-x86_64/column build-x86_64/user/column.o build-x86_64/bomb build-x86_64/user/bomb.o build-x86_64/create build-x86_64/user/create.o build-x86_64/rot13 build-x86_64/user/rot13.o build-x86_64/trace build-x86_64/user/trace.o build-x86_64/top build-x86_64/user/top.o build-x86_64/bf2 build-x86_64/user/bf2.o build-x86_64/insmod build-x86_64/user/insmod.o build-x86_64/head build-x86_64/user/head.o


build-x86_64/libc.a: build-x86_64/libc/crt0.o build-x86_64/libc/crti.o build-x86_64/libc/crtn.o build-x86_64/libc/ctype.o build-x86_64/libc/entry.o build-x86_64/libc/errno.o build-x86_64/libc/fstdio.o build-x86_64/libc/fstdio_unlocked.o build-x86_64/libc/getopt.o build-x86_64/libc/locale.o build-x86_64/libc/malloc.o build-x86_64/libc/nightingale.o build-x86_64/libc/setjmp.o build-x86_64/libc/signal.o build-x86_64/libc/stdio.o build-x86_64/libc/stdlib.o build-x86_64/libc/string.o build-x86_64/libc/strtod.o build-x86_64/libc/syscall.o build-x86_64/libc/syscalls.o build-x86_64/libc/time.o build-x86_64/libc/todo.o build-x86_64/libc/unistd.o build-x86_64/libc/vector.o build-x86_64/libc/x86_64/nightingale.o 
	$(info AR	libc.a)
	@ar rcs -o build-x86_64/libc.a build-x86_64/libc/crt0.o build-x86_64/libc/crti.o build-x86_64/libc/crtn.o build-x86_64/libc/ctype.o build-x86_64/libc/entry.o build-x86_64/libc/errno.o build-x86_64/libc/fstdio.o build-x86_64/libc/fstdio_unlocked.o build-x86_64/libc/getopt.o build-x86_64/libc/locale.o build-x86_64/libc/malloc.o build-x86_64/libc/nightingale.o build-x86_64/libc/setjmp.o build-x86_64/libc/signal.o build-x86_64/libc/stdio.o build-x86_64/libc/stdlib.o build-x86_64/libc/string.o build-x86_64/libc/strtod.o build-x86_64/libc/syscall.o build-x86_64/libc/syscalls.o build-x86_64/libc/time.o build-x86_64/libc/todo.o build-x86_64/libc/unistd.o build-x86_64/libc/vector.o build-x86_64/libc/x86_64/nightingale.o
build-x86_64/libc/crt0.o: libc/crt0.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info AS	crt0.o)
	@gcc  -MD -MF dep/libc/crt0.d -c libc/crt0.S -o build-x86_64/libc/crt0.o
build-x86_64/libc/crti.o: libc/crti.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info AS	crti.o)
	@gcc  -MD -MF dep/libc/crti.d -c libc/crti.S -o build-x86_64/libc/crti.o
build-x86_64/libc/crtn.o: libc/crtn.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info AS	crtn.o)
	@gcc  -MD -MF dep/libc/crtn.d -c libc/crtn.S -o build-x86_64/libc/crtn.o
build-x86_64/libc/ctype.o: libc/ctype.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	ctype.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/ctype.d -c libc/ctype.c -o build-x86_64/libc/ctype.o
build-x86_64/libc/entry.o: libc/entry.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	entry.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/entry.d -c libc/entry.c -o build-x86_64/libc/entry.o
build-x86_64/libc/errno.o: libc/errno.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	errno.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/errno.d -c libc/errno.c -o build-x86_64/libc/errno.o
build-x86_64/libc/fstdio.o: libc/fstdio.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	fstdio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/fstdio.d -c libc/fstdio.c -o build-x86_64/libc/fstdio.o
build-x86_64/libc/fstdio_unlocked.o: libc/fstdio_unlocked.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	fstdio_unlocked.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/fstdio_unlocked.d -c libc/fstdio_unlocked.c -o build-x86_64/libc/fstdio_unlocked.o
build-x86_64/libc/getopt.o: libc/getopt.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	getopt.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/getopt.d -c libc/getopt.c -o build-x86_64/libc/getopt.o
build-x86_64/libc/locale.o: libc/locale.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	locale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/locale.d -c libc/locale.c -o build-x86_64/libc/locale.o
build-x86_64/libc/malloc.o: libc/malloc.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	malloc.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/malloc.d -c libc/malloc.c -o build-x86_64/libc/malloc.o
build-x86_64/libc/nightingale.o: libc/nightingale.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	nightingale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/nightingale.d -c libc/nightingale.c -o build-x86_64/libc/nightingale.o
build-x86_64/libc/setjmp.o: libc/setjmp.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info AS	setjmp.o)
	@gcc  -MD -MF dep/libc/setjmp.d -c libc/setjmp.S -o build-x86_64/libc/setjmp.o
build-x86_64/libc/signal.o: libc/signal.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	signal.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/signal.d -c libc/signal.c -o build-x86_64/libc/signal.o
build-x86_64/libc/stdio.o: libc/stdio.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	stdio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/stdio.d -c libc/stdio.c -o build-x86_64/libc/stdio.o
build-x86_64/libc/stdlib.o: libc/stdlib.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	stdlib.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/stdlib.d -c libc/stdlib.c -o build-x86_64/libc/stdlib.o
build-x86_64/libc/string.o: libc/string.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	string.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/string.d -c libc/string.c -o build-x86_64/libc/string.o
build-x86_64/libc/strtod.o: libc/strtod.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	strtod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/strtod.d -c libc/strtod.c -o build-x86_64/libc/strtod.o
build-x86_64/libc/syscall.o: libc/syscall.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	syscall.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/syscall.d -c libc/syscall.c -o build-x86_64/libc/syscall.o
build-x86_64/libc/syscalls.o: libc/syscalls.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	syscalls.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/syscalls.d -c libc/syscalls.c -o build-x86_64/libc/syscalls.o
build-x86_64/libc/time.o: libc/time.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	time.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/time.d -c libc/time.c -o build-x86_64/libc/time.o
build-x86_64/libc/todo.o: libc/todo.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	todo.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/todo.d -c libc/todo.c -o build-x86_64/libc/todo.o
build-x86_64/libc/unistd.o: libc/unistd.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	unistd.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/unistd.d -c libc/unistd.c -o build-x86_64/libc/unistd.o
build-x86_64/libc/vector.o: libc/vector.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	$(info CC	vector.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/vector.d -c libc/vector.c -o build-x86_64/libc/vector.o
build-x86_64/libc/x86_64/nightingale.o: libc/x86_64/nightingale.c
	@mkdir -p build-x86_64/libc/x86_64
	@mkdir -p dep/libc/x86_64
	$(info CC	nightingale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/libc/x86_64/nightingale.d -c libc/x86_64/nightingale.c -o build-x86_64/libc/x86_64/nightingale.o
build-x86_64/libc.so: build-x86_64/libc_so/crt0.o build-x86_64/libc_so/crti.o build-x86_64/libc_so/crtn.o build-x86_64/libc_so/ctype.o build-x86_64/libc_so/entry.o build-x86_64/libc_so/errno.o build-x86_64/libc_so/fstdio.o build-x86_64/libc_so/fstdio_unlocked.o build-x86_64/libc_so/getopt.o build-x86_64/libc_so/locale.o build-x86_64/libc_so/malloc.o build-x86_64/libc_so/nightingale.o build-x86_64/libc_so/setjmp.o build-x86_64/libc_so/signal.o build-x86_64/libc_so/stdio.o build-x86_64/libc_so/stdlib.o build-x86_64/libc_so/string.o build-x86_64/libc_so/strtod.o build-x86_64/libc_so/syscall.o build-x86_64/libc_so/syscalls.o build-x86_64/libc_so/time.o build-x86_64/libc_so/todo.o build-x86_64/libc_so/unistd.o build-x86_64/libc_so/vector.o build-x86_64/libc_so/x86_64/nightingale.o 
	$(info LD	libc.so)
	@x86_64-nightingale-gcc -nostdlib -fpic -shared -g -o build-x86_64/libc.so build-x86_64/libc_so/crt0.o build-x86_64/libc_so/crti.o build-x86_64/libc_so/crtn.o build-x86_64/libc_so/ctype.o build-x86_64/libc_so/entry.o build-x86_64/libc_so/errno.o build-x86_64/libc_so/fstdio.o build-x86_64/libc_so/fstdio_unlocked.o build-x86_64/libc_so/getopt.o build-x86_64/libc_so/locale.o build-x86_64/libc_so/malloc.o build-x86_64/libc_so/nightingale.o build-x86_64/libc_so/setjmp.o build-x86_64/libc_so/signal.o build-x86_64/libc_so/stdio.o build-x86_64/libc_so/stdlib.o build-x86_64/libc_so/string.o build-x86_64/libc_so/strtod.o build-x86_64/libc_so/syscall.o build-x86_64/libc_so/syscalls.o build-x86_64/libc_so/time.o build-x86_64/libc_so/todo.o build-x86_64/libc_so/unistd.o build-x86_64/libc_so/vector.o build-x86_64/libc_so/x86_64/nightingale.o 
build-x86_64/libc_so/crt0.o: libc/crt0.S
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info AS	crt0.o)
	@gcc  -MD -MF dep/libc_so/crt0.d -c libc/crt0.S -o build-x86_64/libc_so/crt0.o
build-x86_64/libc_so/crti.o: libc/crti.S
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info AS	crti.o)
	@gcc  -MD -MF dep/libc_so/crti.d -c libc/crti.S -o build-x86_64/libc_so/crti.o
build-x86_64/libc_so/crtn.o: libc/crtn.S
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info AS	crtn.o)
	@gcc  -MD -MF dep/libc_so/crtn.d -c libc/crtn.S -o build-x86_64/libc_so/crtn.o
build-x86_64/libc_so/ctype.o: libc/ctype.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	ctype.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/ctype.d -c libc/ctype.c -o build-x86_64/libc_so/ctype.o
build-x86_64/libc_so/entry.o: libc/entry.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	entry.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/entry.d -c libc/entry.c -o build-x86_64/libc_so/entry.o
build-x86_64/libc_so/errno.o: libc/errno.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	errno.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/errno.d -c libc/errno.c -o build-x86_64/libc_so/errno.o
build-x86_64/libc_so/fstdio.o: libc/fstdio.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	fstdio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/fstdio.d -c libc/fstdio.c -o build-x86_64/libc_so/fstdio.o
build-x86_64/libc_so/fstdio_unlocked.o: libc/fstdio_unlocked.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	fstdio_unlocked.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/fstdio_unlocked.d -c libc/fstdio_unlocked.c -o build-x86_64/libc_so/fstdio_unlocked.o
build-x86_64/libc_so/getopt.o: libc/getopt.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	getopt.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/getopt.d -c libc/getopt.c -o build-x86_64/libc_so/getopt.o
build-x86_64/libc_so/locale.o: libc/locale.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	locale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/locale.d -c libc/locale.c -o build-x86_64/libc_so/locale.o
build-x86_64/libc_so/malloc.o: libc/malloc.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	malloc.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/malloc.d -c libc/malloc.c -o build-x86_64/libc_so/malloc.o
build-x86_64/libc_so/nightingale.o: libc/nightingale.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	nightingale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/nightingale.d -c libc/nightingale.c -o build-x86_64/libc_so/nightingale.o
build-x86_64/libc_so/setjmp.o: libc/setjmp.S
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info AS	setjmp.o)
	@gcc  -MD -MF dep/libc_so/setjmp.d -c libc/setjmp.S -o build-x86_64/libc_so/setjmp.o
build-x86_64/libc_so/signal.o: libc/signal.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	signal.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/signal.d -c libc/signal.c -o build-x86_64/libc_so/signal.o
build-x86_64/libc_so/stdio.o: libc/stdio.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	stdio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/stdio.d -c libc/stdio.c -o build-x86_64/libc_so/stdio.o
build-x86_64/libc_so/stdlib.o: libc/stdlib.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	stdlib.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/stdlib.d -c libc/stdlib.c -o build-x86_64/libc_so/stdlib.o
build-x86_64/libc_so/string.o: libc/string.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	string.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/string.d -c libc/string.c -o build-x86_64/libc_so/string.o
build-x86_64/libc_so/strtod.o: libc/strtod.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	strtod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/strtod.d -c libc/strtod.c -o build-x86_64/libc_so/strtod.o
build-x86_64/libc_so/syscall.o: libc/syscall.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	syscall.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/syscall.d -c libc/syscall.c -o build-x86_64/libc_so/syscall.o
build-x86_64/libc_so/syscalls.o: libc/syscalls.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	syscalls.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/syscalls.d -c libc/syscalls.c -o build-x86_64/libc_so/syscalls.o
build-x86_64/libc_so/time.o: libc/time.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	time.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/time.d -c libc/time.c -o build-x86_64/libc_so/time.o
build-x86_64/libc_so/todo.o: libc/todo.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	todo.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/todo.d -c libc/todo.c -o build-x86_64/libc_so/todo.o
build-x86_64/libc_so/unistd.o: libc/unistd.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	unistd.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/unistd.d -c libc/unistd.c -o build-x86_64/libc_so/unistd.o
build-x86_64/libc_so/vector.o: libc/vector.c
	@mkdir -p build-x86_64/libc_so
	@mkdir -p dep/libc_so
	$(info CC	vector.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/vector.d -c libc/vector.c -o build-x86_64/libc_so/vector.o
build-x86_64/libc_so/x86_64/nightingale.o: libc/x86_64/nightingale.c
	@mkdir -p build-x86_64/libc_so/x86_64
	@mkdir -p dep/libc_so/x86_64
	$(info CC	nightingale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/libc_so/x86_64/nightingale.d -c libc/x86_64/nightingale.c -o build-x86_64/libc_so/x86_64/nightingale.o
build-x86_64/libk.a: build-x86_64/libk/ctype.o build-x86_64/libk/errno.o build-x86_64/libk/malloc.o build-x86_64/libk/setjmp.o build-x86_64/libk/signal.o build-x86_64/libk/stdio.o build-x86_64/libk/stdlib.o build-x86_64/libk/string.o build-x86_64/libk/x86_64/nightingale.o 
	$(info AR	libk.a)
	@ar rcs -o build-x86_64/libk.a build-x86_64/libk/ctype.o build-x86_64/libk/errno.o build-x86_64/libk/malloc.o build-x86_64/libk/setjmp.o build-x86_64/libk/signal.o build-x86_64/libk/stdio.o build-x86_64/libk/stdlib.o build-x86_64/libk/string.o build-x86_64/libk/x86_64/nightingale.o
build-x86_64/libk/ctype.o: libc/ctype.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(info CC	ctype.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/ctype.d -c libc/ctype.c -o build-x86_64/libk/ctype.o
build-x86_64/libk/errno.o: libc/errno.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(info CC	errno.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/errno.d -c libc/errno.c -o build-x86_64/libk/errno.o
build-x86_64/libk/malloc.o: libc/malloc.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(info CC	malloc.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/malloc.d -c libc/malloc.c -o build-x86_64/libk/malloc.o
build-x86_64/libk/setjmp.o: libc/setjmp.S
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(info AS	setjmp.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/setjmp.d -c libc/setjmp.S -o build-x86_64/libk/setjmp.o
build-x86_64/libk/signal.o: libc/signal.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(info CC	signal.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/signal.d -c libc/signal.c -o build-x86_64/libk/signal.o
build-x86_64/libk/stdio.o: libc/stdio.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(info CC	stdio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/stdio.d -c libc/stdio.c -o build-x86_64/libk/stdio.o
build-x86_64/libk/stdlib.o: libc/stdlib.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(info CC	stdlib.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/stdlib.d -c libc/stdlib.c -o build-x86_64/libk/stdlib.o
build-x86_64/libk/string.o: libc/string.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	$(info CC	string.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/string.d -c libc/string.c -o build-x86_64/libk/string.o
build-x86_64/libk/x86_64/nightingale.o: libc/x86_64/nightingale.c
	@mkdir -p build-x86_64/libk/x86_64
	@mkdir -p dep/libk/x86_64
	$(info CC	nightingale.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/x86_64/nightingale.d -c libc/x86_64/nightingale.c -o build-x86_64/libk/x86_64/nightingale.o
build-x86_64/ngk.elf: build-x86_64/fs/char_devices.o build-x86_64/fs/directory.o build-x86_64/fs/fs.o build-x86_64/fs/membuf.o build-x86_64/fs/pipe.o build-x86_64/fs/procfs.o build-x86_64/fs/socket.o build-x86_64/kernel/debug.o build-x86_64/kernel/dmgr.o build-x86_64/kernel/elf.o build-x86_64/kernel/exec.o build-x86_64/kernel/irq.o build-x86_64/kernel/main.o build-x86_64/kernel/mman.o build-x86_64/kernel/mod.o build-x86_64/kernel/multiboot.o build-x86_64/kernel/pci.o build-x86_64/kernel/pmm.o build-x86_64/kernel/rand.o build-x86_64/kernel/ringbuf.o build-x86_64/kernel/serial.o build-x86_64/kernel/signal.o build-x86_64/kernel/spalloc.o build-x86_64/kernel/string.o build-x86_64/kernel/sync.o build-x86_64/kernel/sync_testbed.o build-x86_64/kernel/syscall.o build-x86_64/kernel/tarfs.o build-x86_64/kernel/tests.o build-x86_64/kernel/thread.o build-x86_64/kernel/timer.o build-x86_64/kernel/trace.o build-x86_64/kernel/tty.o build-x86_64/kernel/ubsan.o build-x86_64/kernel/uname.o build-x86_64/linker/elf-ng.o build-x86_64/linker/modld.o build-x86_64/x86/acpi.o build-x86_64/x86/apic.o build-x86_64/x86/boot.o build-x86_64/x86/cpu.o build-x86_64/x86/halt.o build-x86_64/x86/interrupt.o build-x86_64/x86/isrs.o build-x86_64/x86/pic.o build-x86_64/x86/pit.o build-x86_64/x86/uart.o build-x86_64/x86/vmm.o build-x86_64/libk.a
	$(info LD	ngk.elf)
	@x86_64-nightingale-gcc -nostdlib -Tx86/link_hh.ld -zmax-page-size=0x1000 -g -Lbuild-x86_64 -o build-x86_64/ngk.elf build-x86_64/fs/char_devices.o build-x86_64/fs/directory.o build-x86_64/fs/fs.o build-x86_64/fs/membuf.o build-x86_64/fs/pipe.o build-x86_64/fs/procfs.o build-x86_64/fs/socket.o build-x86_64/kernel/debug.o build-x86_64/kernel/dmgr.o build-x86_64/kernel/elf.o build-x86_64/kernel/exec.o build-x86_64/kernel/irq.o build-x86_64/kernel/main.o build-x86_64/kernel/mman.o build-x86_64/kernel/mod.o build-x86_64/kernel/multiboot.o build-x86_64/kernel/pci.o build-x86_64/kernel/pmm.o build-x86_64/kernel/rand.o build-x86_64/kernel/ringbuf.o build-x86_64/kernel/serial.o build-x86_64/kernel/signal.o build-x86_64/kernel/spalloc.o build-x86_64/kernel/string.o build-x86_64/kernel/sync.o build-x86_64/kernel/sync_testbed.o build-x86_64/kernel/syscall.o build-x86_64/kernel/tarfs.o build-x86_64/kernel/tests.o build-x86_64/kernel/thread.o build-x86_64/kernel/timer.o build-x86_64/kernel/trace.o build-x86_64/kernel/tty.o build-x86_64/kernel/ubsan.o build-x86_64/kernel/uname.o build-x86_64/linker/elf-ng.o build-x86_64/linker/modld.o build-x86_64/x86/acpi.o build-x86_64/x86/apic.o build-x86_64/x86/boot.o build-x86_64/x86/cpu.o build-x86_64/x86/halt.o build-x86_64/x86/interrupt.o build-x86_64/x86/isrs.o build-x86_64/x86/pic.o build-x86_64/x86/pit.o build-x86_64/x86/uart.o build-x86_64/x86/vmm.o -lk -lgcc
build-x86_64/fs/char_devices.o: fs/char_devices.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(info CC	char_devices.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/fs/char_devices.d -c fs/char_devices.c -o build-x86_64/fs/char_devices.o
build-x86_64/fs/directory.o: fs/directory.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(info CC	directory.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/fs/directory.d -c fs/directory.c -o build-x86_64/fs/directory.o
build-x86_64/fs/fs.o: fs/fs.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(info CC	fs.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/fs/fs.d -c fs/fs.c -o build-x86_64/fs/fs.o
build-x86_64/fs/membuf.o: fs/membuf.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(info CC	membuf.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/fs/membuf.d -c fs/membuf.c -o build-x86_64/fs/membuf.o
build-x86_64/fs/pipe.o: fs/pipe.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(info CC	pipe.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/fs/pipe.d -c fs/pipe.c -o build-x86_64/fs/pipe.o
build-x86_64/fs/procfs.o: fs/procfs.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(info CC	procfs.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/fs/procfs.d -c fs/procfs.c -o build-x86_64/fs/procfs.o
build-x86_64/fs/socket.o: fs/socket.c
	@mkdir -p build-x86_64/fs
	@mkdir -p dep/fs
	$(info CC	socket.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/fs/socket.d -c fs/socket.c -o build-x86_64/fs/socket.o
build-x86_64/kernel/debug.o: kernel/debug.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	debug.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/debug.d -c kernel/debug.c -o build-x86_64/kernel/debug.o
build-x86_64/kernel/dmgr.o: kernel/dmgr.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	dmgr.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/dmgr.d -c kernel/dmgr.c -o build-x86_64/kernel/dmgr.o
build-x86_64/kernel/elf.o: kernel/elf.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	elf.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/elf.d -c kernel/elf.c -o build-x86_64/kernel/elf.o
build-x86_64/kernel/exec.o: kernel/exec.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	exec.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/exec.d -c kernel/exec.c -o build-x86_64/kernel/exec.o
build-x86_64/kernel/irq.o: kernel/irq.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	irq.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/irq.d -c kernel/irq.c -o build-x86_64/kernel/irq.o
build-x86_64/kernel/main.o: kernel/main.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	main.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/main.d -c kernel/main.c -o build-x86_64/kernel/main.o
build-x86_64/kernel/mman.o: kernel/mman.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	mman.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/mman.d -c kernel/mman.c -o build-x86_64/kernel/mman.o
build-x86_64/kernel/mod.o: kernel/mod.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	mod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/mod.d -c kernel/mod.c -o build-x86_64/kernel/mod.o
build-x86_64/kernel/multiboot.o: kernel/multiboot.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	multiboot.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/multiboot.d -c kernel/multiboot.c -o build-x86_64/kernel/multiboot.o
build-x86_64/kernel/pci.o: kernel/pci.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	pci.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/pci.d -c kernel/pci.c -o build-x86_64/kernel/pci.o
build-x86_64/kernel/pmm.o: kernel/pmm.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	pmm.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/pmm.d -c kernel/pmm.c -o build-x86_64/kernel/pmm.o
build-x86_64/kernel/rand.o: kernel/rand.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	rand.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/rand.d -c kernel/rand.c -o build-x86_64/kernel/rand.o
build-x86_64/kernel/ringbuf.o: kernel/ringbuf.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	ringbuf.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/ringbuf.d -c kernel/ringbuf.c -o build-x86_64/kernel/ringbuf.o
build-x86_64/kernel/serial.o: kernel/serial.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	serial.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/serial.d -c kernel/serial.c -o build-x86_64/kernel/serial.o
build-x86_64/kernel/signal.o: kernel/signal.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	signal.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/signal.d -c kernel/signal.c -o build-x86_64/kernel/signal.o
build-x86_64/kernel/spalloc.o: kernel/spalloc.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	spalloc.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/spalloc.d -c kernel/spalloc.c -o build-x86_64/kernel/spalloc.o
build-x86_64/kernel/string.o: kernel/string.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	string.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/string.d -c kernel/string.c -o build-x86_64/kernel/string.o
build-x86_64/kernel/sync.o: kernel/sync.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	sync.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/sync.d -c kernel/sync.c -o build-x86_64/kernel/sync.o
build-x86_64/kernel/sync_testbed.o: kernel/sync_testbed.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	sync_testbed.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/sync_testbed.d -c kernel/sync_testbed.c -o build-x86_64/kernel/sync_testbed.o
build-x86_64/kernel/syscall.o: kernel/syscall.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	syscall.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/syscall.d -c kernel/syscall.c -o build-x86_64/kernel/syscall.o
build-x86_64/kernel/tarfs.o: kernel/tarfs.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	tarfs.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/tarfs.d -c kernel/tarfs.c -o build-x86_64/kernel/tarfs.o
build-x86_64/kernel/tests.o: kernel/tests.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	tests.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/tests.d -c kernel/tests.c -o build-x86_64/kernel/tests.o
build-x86_64/kernel/thread.o: kernel/thread.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	thread.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/thread.d -c kernel/thread.c -o build-x86_64/kernel/thread.o
build-x86_64/kernel/timer.o: kernel/timer.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	timer.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/timer.d -c kernel/timer.c -o build-x86_64/kernel/timer.o
build-x86_64/kernel/trace.o: kernel/trace.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	trace.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/trace.d -c kernel/trace.c -o build-x86_64/kernel/trace.o
build-x86_64/kernel/tty.o: kernel/tty.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	tty.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/tty.d -c kernel/tty.c -o build-x86_64/kernel/tty.o
build-x86_64/kernel/ubsan.o: kernel/ubsan.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	ubsan.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/ubsan.d -c kernel/ubsan.c -o build-x86_64/kernel/ubsan.o
build-x86_64/kernel/uname.o: kernel/uname.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	$(info CC	uname.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/uname.d -c kernel/uname.c -o build-x86_64/kernel/uname.o
build-x86_64/linker/elf-ng.o: linker/elf-ng.c
	@mkdir -p build-x86_64/linker
	@mkdir -p dep/linker
	$(info CC	elf-ng.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/linker/elf-ng.d -c linker/elf-ng.c -o build-x86_64/linker/elf-ng.o
build-x86_64/linker/modld.o: linker/modld.c
	@mkdir -p build-x86_64/linker
	@mkdir -p dep/linker
	$(info CC	modld.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/linker/modld.d -c linker/modld.c -o build-x86_64/linker/modld.o
build-x86_64/x86/acpi.o: x86/acpi.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info CC	acpi.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/acpi.d -c x86/acpi.c -o build-x86_64/x86/acpi.o
build-x86_64/x86/apic.o: x86/apic.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info CC	apic.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/apic.d -c x86/apic.c -o build-x86_64/x86/apic.o
build-x86_64/x86/boot.o: x86/boot.asm
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info NASM	boot.o)
	@nasm -felf64 x86/boot.asm -o build-x86_64/x86/boot.o
build-x86_64/x86/cpu.o: x86/cpu.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info CC	cpu.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/cpu.d -c x86/cpu.c -o build-x86_64/x86/cpu.o
build-x86_64/x86/halt.o: x86/halt.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info CC	halt.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/halt.d -c x86/halt.c -o build-x86_64/x86/halt.o
build-x86_64/x86/interrupt.o: x86/interrupt.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info CC	interrupt.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/interrupt.d -c x86/interrupt.c -o build-x86_64/x86/interrupt.o
build-x86_64/x86/isrs.o: x86/isrs.asm
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info NASM	isrs.o)
	@nasm -felf64 x86/isrs.asm -o build-x86_64/x86/isrs.o
build-x86_64/x86/pic.o: x86/pic.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info CC	pic.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/pic.d -c x86/pic.c -o build-x86_64/x86/pic.o
build-x86_64/x86/pit.o: x86/pit.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info CC	pit.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/pit.d -c x86/pit.c -o build-x86_64/x86/pit.o
build-x86_64/x86/uart.o: x86/uart.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info CC	uart.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/uart.d -c x86/uart.c -o build-x86_64/x86/uart.o
build-x86_64/x86/vmm.o: x86/vmm.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	$(info CC	vmm.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/vmm.d -c x86/vmm.c -o build-x86_64/x86/vmm.o
build-x86_64/crt0.o: build-x86_64/crt/crt0.o
	$(info CP	crt0.o)
	@cp --preserve=timestamps build-x86_64/crt/crt0.o build-x86_64/crt0.o
build-x86_64/crt/crt0.o: libc/crt0.S
	@mkdir -p build-x86_64/crt
	@mkdir -p dep/crt
	$(info AS	crt0.o)
	@x86_64-nightingale-gcc  -MD -MF dep/crt/crt0.d -c libc/crt0.S -o build-x86_64/crt/crt0.o
build-x86_64/sh: build-x86_64/sh-/parse.o build-x86_64/sh-/readline.o build-x86_64/sh-/sh.o build-x86_64/sh-/token.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	sh)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/sh build-x86_64/sh-/parse.o build-x86_64/sh-/readline.o build-x86_64/sh-/sh.o build-x86_64/sh-/token.o 
build-x86_64/sh-/parse.o: sh/parse.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	$(info CC	parse.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/sh-/parse.d -c sh/parse.c -o build-x86_64/sh-/parse.o
build-x86_64/sh-/readline.o: sh/readline.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	$(info CC	readline.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/sh-/readline.d -c sh/readline.c -o build-x86_64/sh-/readline.o
build-x86_64/sh-/sh.o: sh/sh.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	$(info CC	sh.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/sh-/sh.d -c sh/sh.c -o build-x86_64/sh-/sh.o
build-x86_64/sh-/token.o: sh/token.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	$(info CC	token.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/sh-/token.d -c sh/token.c -o build-x86_64/sh-/token.o
build-x86_64/ld-ng.so: build-x86_64/ld-ng/elf-ng.o build-x86_64/ld-ng/ldso.o 
	$(info LD	ld-ng.so)
	@x86_64-nightingale-gcc -nostdlib -fpic -shared -g -o build-x86_64/ld-ng.so build-x86_64/ld-ng/elf-ng.o build-x86_64/ld-ng/ldso.o 
build-x86_64/ld-ng/elf-ng.o: linker/elf-ng.c
	@mkdir -p build-x86_64/ld-ng
	@mkdir -p dep/ld-ng
	$(info CC	elf-ng.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/ld-ng/elf-ng.d -c linker/elf-ng.c -o build-x86_64/ld-ng/elf-ng.o
build-x86_64/ld-ng/ldso.o: linker/ldso.c
	@mkdir -p build-x86_64/ld-ng
	@mkdir -p dep/ld-ng
	$(info CC	ldso.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -g -Og -nostdlib -fpic -shared -MD -MF dep/ld-ng/ldso.d -c linker/ldso.c -o build-x86_64/ld-ng/ldso.o
build-x86_64/testmod.ko: build-x86_64/modules/testmod.o
	$(info CP	testmod.ko)
	@cp --preserve=timestamps build-x86_64/modules/testmod.o build-x86_64/testmod.ko
build-x86_64/modules/testmod.o: modules/testmod.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	$(info CC	testmod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/modules/testmod.d -c modules/testmod.c -o build-x86_64/modules/testmod.o
build-x86_64/thread.ko: build-x86_64/modules/thread.o
	$(info CP	thread.ko)
	@cp --preserve=timestamps build-x86_64/modules/thread.o build-x86_64/thread.ko
build-x86_64/modules/thread.o: modules/thread.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	$(info CC	thread.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/modules/thread.d -c modules/thread.c -o build-x86_64/modules/thread.o
build-x86_64/syscall.ko: build-x86_64/modules/syscall.o
	$(info CP	syscall.ko)
	@cp --preserve=timestamps build-x86_64/modules/syscall.o build-x86_64/syscall.ko
build-x86_64/modules/syscall.o: modules/syscall.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	$(info CC	syscall.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"$(NIGHTINGALE_VERSION)\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/modules/syscall.d -c modules/syscall.c -o build-x86_64/modules/syscall.o
build-x86_64/threads: build-x86_64/user/threads.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	threads)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/threads build-x86_64/user/threads.o 
build-x86_64/user/threads.o: user/threads.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	threads.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/threads.d -c user/threads.c -o build-x86_64/user/threads.o
build-x86_64/hog: build-x86_64/user/hog.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	hog)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/hog build-x86_64/user/hog.o 
build-x86_64/user/hog.o: user/hog.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	hog.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/hog.d -c user/hog.c -o build-x86_64/user/hog.o
build-x86_64/echo: build-x86_64/user/echo.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	echo)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/echo build-x86_64/user/echo.o 
build-x86_64/user/echo.o: user/echo.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	echo.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/echo.d -c user/echo.c -o build-x86_64/user/echo.o
build-x86_64/oom: build-x86_64/user/oom.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	oom)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/oom build-x86_64/user/oom.o 
build-x86_64/user/oom.o: user/oom.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	oom.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/oom.d -c user/oom.c -o build-x86_64/user/oom.o
build-x86_64/args: build-x86_64/user/args.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	args)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/args build-x86_64/user/args.o 
build-x86_64/user/args.o: user/args.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	args.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/args.d -c user/args.c -o build-x86_64/user/args.o
build-x86_64/ls: build-x86_64/user/ls.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	ls)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/ls build-x86_64/user/ls.o 
build-x86_64/user/ls.o: user/ls.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	ls.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/ls.d -c user/ls.c -o build-x86_64/user/ls.o
build-x86_64/modsys: build-x86_64/user/modsys.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	modsys)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/modsys build-x86_64/user/modsys.o 
build-x86_64/user/modsys.o: user/modsys.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	modsys.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/modsys.d -c user/modsys.c -o build-x86_64/user/modsys.o
build-x86_64/fio: build-x86_64/user/fio.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	fio)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/fio build-x86_64/user/fio.o 
build-x86_64/user/fio.o: user/fio.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	fio.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/fio.d -c user/fio.c -o build-x86_64/user/fio.o
build-x86_64/xd: build-x86_64/user/xd.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	xd)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/xd build-x86_64/user/xd.o 
build-x86_64/user/xd.o: user/xd.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	xd.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/xd.d -c user/xd.c -o build-x86_64/user/xd.o
build-x86_64/busy: build-x86_64/user/busy.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	busy)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/busy build-x86_64/user/busy.o 
build-x86_64/user/busy.o: user/busy.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	busy.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/busy.d -c user/busy.c -o build-x86_64/user/busy.o
build-x86_64/uthread: build-x86_64/user/uthread.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	uthread)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/uthread build-x86_64/user/uthread.o 
build-x86_64/user/uthread.o: user/uthread.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	uthread.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/uthread.d -c user/uthread.c -o build-x86_64/user/uthread.o
build-x86_64/false: build-x86_64/user/false.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	false)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/false build-x86_64/user/false.o 
build-x86_64/user/false.o: user/false.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	false.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/false.d -c user/false.c -o build-x86_64/user/false.o
build-x86_64/crash: build-x86_64/user/crash.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	crash)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/crash build-x86_64/user/crash.o 
build-x86_64/user/crash.o: user/crash.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	crash.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/crash.d -c user/crash.c -o build-x86_64/user/crash.o
build-x86_64/sleep: build-x86_64/user/sleep.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	sleep)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/sleep build-x86_64/user/sleep.o 
build-x86_64/user/sleep.o: user/sleep.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	sleep.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/sleep.d -c user/sleep.c -o build-x86_64/user/sleep.o
build-x86_64/fcat: build-x86_64/user/fcat.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	fcat)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/fcat build-x86_64/user/fcat.o 
build-x86_64/user/fcat.o: user/fcat.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	fcat.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/fcat.d -c user/fcat.c -o build-x86_64/user/fcat.o
build-x86_64/time: build-x86_64/user/time.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	time)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/time build-x86_64/user/time.o 
build-x86_64/user/time.o: user/time.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	time.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/time.d -c user/time.c -o build-x86_64/user/time.o
build-x86_64/uname: build-x86_64/user/uname.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	uname)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/uname build-x86_64/user/uname.o 
build-x86_64/user/uname.o: user/uname.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	uname.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/uname.d -c user/uname.c -o build-x86_64/user/uname.o
build-x86_64/kill: build-x86_64/user/kill.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	kill)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/kill build-x86_64/user/kill.o 
build-x86_64/user/kill.o: user/kill.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	kill.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/kill.d -c user/kill.c -o build-x86_64/user/kill.o
build-x86_64/syscall_test: build-x86_64/user/syscall_test.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	syscall_test)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/syscall_test build-x86_64/user/syscall_test.o 
build-x86_64/user/syscall_test.o: user/syscall_test.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	syscall_test.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/syscall_test.d -c user/syscall_test.c -o build-x86_64/user/syscall_test.o
build-x86_64/ab: build-x86_64/user/ab.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	ab)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/ab build-x86_64/user/ab.o 
build-x86_64/user/ab.o: user/ab.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	ab.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/ab.d -c user/ab.c -o build-x86_64/user/ab.o
build-x86_64/test: build-x86_64/user/test.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	test)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/test build-x86_64/user/test.o 
build-x86_64/user/test.o: user/test.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	test.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/test.d -c user/test.c -o build-x86_64/user/test.o
build-x86_64/strace: build-x86_64/user/strace.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	strace)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/strace build-x86_64/user/strace.o 
build-x86_64/user/strace.o: user/strace.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	strace.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/strace.d -c user/strace.c -o build-x86_64/user/strace.o
build-x86_64/forks: build-x86_64/user/forks.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	forks)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/forks build-x86_64/user/forks.o 
build-x86_64/user/forks.o: user/forks.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	forks.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/forks.d -c user/forks.c -o build-x86_64/user/forks.o
build-x86_64/bg: build-x86_64/user/bg.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	bg)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/bg build-x86_64/user/bg.o 
build-x86_64/user/bg.o: user/bg.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	bg.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/bg.d -c user/bg.c -o build-x86_64/user/bg.o
build-x86_64/float: build-x86_64/user/float.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	float)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/float build-x86_64/user/float.o 
build-x86_64/user/float.o: user/float.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	float.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/float.d -c user/float.c -o build-x86_64/user/float.o
build-x86_64/clone: build-x86_64/user/clone.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	clone)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/clone build-x86_64/user/clone.o 
build-x86_64/user/clone.o: user/clone.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	clone.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/clone.d -c user/clone.c -o build-x86_64/user/clone.o
build-x86_64/init: build-x86_64/user/init.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	init)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/init build-x86_64/user/init.o 
build-x86_64/user/init.o: user/init.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	init.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/init.d -c user/init.c -o build-x86_64/user/init.o
build-x86_64/mmap: build-x86_64/user/mmap.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	mmap)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/mmap build-x86_64/user/mmap.o 
build-x86_64/user/mmap.o: user/mmap.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	mmap.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/mmap.d -c user/mmap.c -o build-x86_64/user/mmap.o
build-x86_64/cat: build-x86_64/user/cat.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	cat)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/cat build-x86_64/user/cat.o 
build-x86_64/user/cat.o: user/cat.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	cat.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/cat.d -c user/cat.c -o build-x86_64/user/cat.o
build-x86_64/bf: build-x86_64/user/bf.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	bf)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/bf build-x86_64/user/bf.o 
build-x86_64/user/bf.o: user/bf.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	bf.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/bf.d -c user/bf.c -o build-x86_64/user/bf.o
build-x86_64/column: build-x86_64/user/column.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	column)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/column build-x86_64/user/column.o 
build-x86_64/user/column.o: user/column.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	column.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/column.d -c user/column.c -o build-x86_64/user/column.o
build-x86_64/bomb: build-x86_64/user/bomb.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	bomb)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/bomb build-x86_64/user/bomb.o 
build-x86_64/user/bomb.o: user/bomb.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	bomb.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/bomb.d -c user/bomb.c -o build-x86_64/user/bomb.o
build-x86_64/create: build-x86_64/user/create.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	create)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/create build-x86_64/user/create.o 
build-x86_64/user/create.o: user/create.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	create.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/create.d -c user/create.c -o build-x86_64/user/create.o
build-x86_64/rot13: build-x86_64/user/rot13.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	rot13)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/rot13 build-x86_64/user/rot13.o 
build-x86_64/user/rot13.o: user/rot13.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	rot13.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/rot13.d -c user/rot13.c -o build-x86_64/user/rot13.o
build-x86_64/trace: build-x86_64/user/trace.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	trace)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/trace build-x86_64/user/trace.o 
build-x86_64/user/trace.o: user/trace.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	trace.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/trace.d -c user/trace.c -o build-x86_64/user/trace.o
build-x86_64/top: build-x86_64/user/top.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	top)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/top build-x86_64/user/top.o 
build-x86_64/user/top.o: user/top.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	top.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/top.d -c user/top.c -o build-x86_64/user/top.o
build-x86_64/bf2: build-x86_64/user/bf2.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	bf2)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/bf2 build-x86_64/user/bf2.o 
build-x86_64/user/bf2.o: user/bf2.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	bf2.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/bf2.d -c user/bf2.c -o build-x86_64/user/bf2.o
build-x86_64/insmod: build-x86_64/user/insmod.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	insmod)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/insmod build-x86_64/user/insmod.o 
build-x86_64/user/insmod.o: user/insmod.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	insmod.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/insmod.d -c user/insmod.c -o build-x86_64/user/insmod.o
build-x86_64/head: build-x86_64/user/head.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	$(info LD	head)
	@x86_64-nightingale-gcc -g -static -o build-x86_64/head build-x86_64/user/head.o 
build-x86_64/user/head.o: user/head.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	$(info CC	head.o)
	@x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/head.d -c user/head.c -o build-x86_64/user/head.o

sysroot/usr/lib/libc.a: build-x86_64/libc.a
	$(info install	libc.a)
	@cp build-x86_64/libc.a sysroot/usr/lib/libc.a

sysroot/usr/bin/libc.so: build-x86_64/libc.so
	$(info install	libc.so)
	@cp build-x86_64/libc.so sysroot/usr/bin/libc.so

ngk.elf: build-x86_64/ngk.elf
	$(info install	ngk.elf)
	@cp build-x86_64/ngk.elf ngk.elf

sysroot/usr/lib/crt0.o: build-x86_64/crt0.o
	$(info install	crt0.o)
	@cp build-x86_64/crt0.o sysroot/usr/lib/crt0.o

sysroot/usr/bin/sh: build-x86_64/sh
	$(info install	sh)
	@cp build-x86_64/sh sysroot/usr/bin/sh

sysroot/usr/bin/ld-ng.so: build-x86_64/ld-ng.so
	$(info install	ld-ng.so)
	@cp build-x86_64/ld-ng.so sysroot/usr/bin/ld-ng.so

sysroot/usr/bin/testmod.ko: build-x86_64/testmod.ko
	$(info install	testmod.ko)
	@cp build-x86_64/testmod.ko sysroot/usr/bin/testmod.ko

sysroot/usr/bin/thread.ko: build-x86_64/thread.ko
	$(info install	thread.ko)
	@cp build-x86_64/thread.ko sysroot/usr/bin/thread.ko

sysroot/usr/bin/syscall.ko: build-x86_64/syscall.ko
	$(info install	syscall.ko)
	@cp build-x86_64/syscall.ko sysroot/usr/bin/syscall.ko

sysroot/usr/bin/threads: build-x86_64/threads
	$(info install	threads)
	@cp build-x86_64/threads sysroot/usr/bin/threads

sysroot/usr/bin/hog: build-x86_64/hog
	$(info install	hog)
	@cp build-x86_64/hog sysroot/usr/bin/hog

sysroot/usr/bin/echo: build-x86_64/echo
	$(info install	echo)
	@cp build-x86_64/echo sysroot/usr/bin/echo

sysroot/usr/bin/oom: build-x86_64/oom
	$(info install	oom)
	@cp build-x86_64/oom sysroot/usr/bin/oom

sysroot/usr/bin/args: build-x86_64/args
	$(info install	args)
	@cp build-x86_64/args sysroot/usr/bin/args

sysroot/usr/bin/ls: build-x86_64/ls
	$(info install	ls)
	@cp build-x86_64/ls sysroot/usr/bin/ls

sysroot/usr/bin/modsys: build-x86_64/modsys
	$(info install	modsys)
	@cp build-x86_64/modsys sysroot/usr/bin/modsys

sysroot/usr/bin/fio: build-x86_64/fio
	$(info install	fio)
	@cp build-x86_64/fio sysroot/usr/bin/fio

sysroot/usr/bin/xd: build-x86_64/xd
	$(info install	xd)
	@cp build-x86_64/xd sysroot/usr/bin/xd

sysroot/usr/bin/busy: build-x86_64/busy
	$(info install	busy)
	@cp build-x86_64/busy sysroot/usr/bin/busy

sysroot/usr/bin/uthread: build-x86_64/uthread
	$(info install	uthread)
	@cp build-x86_64/uthread sysroot/usr/bin/uthread

sysroot/usr/bin/false: build-x86_64/false
	$(info install	false)
	@cp build-x86_64/false sysroot/usr/bin/false

sysroot/usr/bin/crash: build-x86_64/crash
	$(info install	crash)
	@cp build-x86_64/crash sysroot/usr/bin/crash

sysroot/usr/bin/sleep: build-x86_64/sleep
	$(info install	sleep)
	@cp build-x86_64/sleep sysroot/usr/bin/sleep

sysroot/usr/bin/fcat: build-x86_64/fcat
	$(info install	fcat)
	@cp build-x86_64/fcat sysroot/usr/bin/fcat

sysroot/usr/bin/time: build-x86_64/time
	$(info install	time)
	@cp build-x86_64/time sysroot/usr/bin/time

sysroot/usr/bin/uname: build-x86_64/uname
	$(info install	uname)
	@cp build-x86_64/uname sysroot/usr/bin/uname

sysroot/usr/bin/kill: build-x86_64/kill
	$(info install	kill)
	@cp build-x86_64/kill sysroot/usr/bin/kill

sysroot/usr/bin/syscall_test: build-x86_64/syscall_test
	$(info install	syscall_test)
	@cp build-x86_64/syscall_test sysroot/usr/bin/syscall_test

sysroot/usr/bin/ab: build-x86_64/ab
	$(info install	ab)
	@cp build-x86_64/ab sysroot/usr/bin/ab

sysroot/usr/bin/test: build-x86_64/test
	$(info install	test)
	@cp build-x86_64/test sysroot/usr/bin/test

sysroot/usr/bin/strace: build-x86_64/strace
	$(info install	strace)
	@cp build-x86_64/strace sysroot/usr/bin/strace

sysroot/usr/bin/forks: build-x86_64/forks
	$(info install	forks)
	@cp build-x86_64/forks sysroot/usr/bin/forks

sysroot/usr/bin/bg: build-x86_64/bg
	$(info install	bg)
	@cp build-x86_64/bg sysroot/usr/bin/bg

sysroot/usr/bin/float: build-x86_64/float
	$(info install	float)
	@cp build-x86_64/float sysroot/usr/bin/float

sysroot/usr/bin/clone: build-x86_64/clone
	$(info install	clone)
	@cp build-x86_64/clone sysroot/usr/bin/clone

sysroot/usr/bin/init: build-x86_64/init
	$(info install	init)
	@cp build-x86_64/init sysroot/usr/bin/init

sysroot/usr/bin/mmap: build-x86_64/mmap
	$(info install	mmap)
	@cp build-x86_64/mmap sysroot/usr/bin/mmap

sysroot/usr/bin/cat: build-x86_64/cat
	$(info install	cat)
	@cp build-x86_64/cat sysroot/usr/bin/cat

sysroot/usr/bin/bf: build-x86_64/bf
	$(info install	bf)
	@cp build-x86_64/bf sysroot/usr/bin/bf

sysroot/usr/bin/column: build-x86_64/column
	$(info install	column)
	@cp build-x86_64/column sysroot/usr/bin/column

sysroot/usr/bin/bomb: build-x86_64/bomb
	$(info install	bomb)
	@cp build-x86_64/bomb sysroot/usr/bin/bomb

sysroot/usr/bin/create: build-x86_64/create
	$(info install	create)
	@cp build-x86_64/create sysroot/usr/bin/create

sysroot/usr/bin/rot13: build-x86_64/rot13
	$(info install	rot13)
	@cp build-x86_64/rot13 sysroot/usr/bin/rot13

sysroot/usr/bin/trace: build-x86_64/trace
	$(info install	trace)
	@cp build-x86_64/trace sysroot/usr/bin/trace

sysroot/usr/bin/top: build-x86_64/top
	$(info install	top)
	@cp build-x86_64/top sysroot/usr/bin/top

sysroot/usr/bin/bf2: build-x86_64/bf2
	$(info install	bf2)
	@cp build-x86_64/bf2 sysroot/usr/bin/bf2

sysroot/usr/bin/insmod: build-x86_64/insmod
	$(info install	insmod)
	@cp build-x86_64/insmod sysroot/usr/bin/insmod

sysroot/usr/bin/head: build-x86_64/head
	$(info install	head)
	@cp build-x86_64/head sysroot/usr/bin/head


mp_install: sysroot/usr/lib/libc.a sysroot/usr/bin/libc.so build-x86_64/libk.a ngk.elf sysroot/usr/lib/crt0.o sysroot/usr/bin/sh sysroot/usr/bin/ld-ng.so sysroot/usr/bin/testmod.ko sysroot/usr/bin/thread.ko sysroot/usr/bin/syscall.ko sysroot/usr/bin/threads sysroot/usr/bin/hog sysroot/usr/bin/echo sysroot/usr/bin/oom sysroot/usr/bin/args sysroot/usr/bin/ls sysroot/usr/bin/modsys sysroot/usr/bin/fio sysroot/usr/bin/xd sysroot/usr/bin/busy sysroot/usr/bin/uthread sysroot/usr/bin/false sysroot/usr/bin/crash sysroot/usr/bin/sleep sysroot/usr/bin/fcat sysroot/usr/bin/time sysroot/usr/bin/uname sysroot/usr/bin/kill sysroot/usr/bin/syscall_test sysroot/usr/bin/ab sysroot/usr/bin/test sysroot/usr/bin/strace sysroot/usr/bin/forks sysroot/usr/bin/bg sysroot/usr/bin/float sysroot/usr/bin/clone sysroot/usr/bin/init sysroot/usr/bin/mmap sysroot/usr/bin/cat sysroot/usr/bin/bf sysroot/usr/bin/column sysroot/usr/bin/bomb sysroot/usr/bin/create sysroot/usr/bin/rot13 sysroot/usr/bin/trace sysroot/usr/bin/top sysroot/usr/bin/bf2 sysroot/usr/bin/insmod sysroot/usr/bin/head

MP_ALL_INSTALL_TARGETS := sysroot/usr/lib/libc.a sysroot/usr/bin/libc.so ngk.elf sysroot/usr/lib/crt0.o sysroot/usr/bin/sh sysroot/usr/bin/ld-ng.so sysroot/usr/bin/testmod.ko sysroot/usr/bin/thread.ko sysroot/usr/bin/syscall.ko sysroot/usr/bin/threads sysroot/usr/bin/hog sysroot/usr/bin/echo sysroot/usr/bin/oom sysroot/usr/bin/args sysroot/usr/bin/ls sysroot/usr/bin/modsys sysroot/usr/bin/fio sysroot/usr/bin/xd sysroot/usr/bin/busy sysroot/usr/bin/uthread sysroot/usr/bin/false sysroot/usr/bin/crash sysroot/usr/bin/sleep sysroot/usr/bin/fcat sysroot/usr/bin/time sysroot/usr/bin/uname sysroot/usr/bin/kill sysroot/usr/bin/syscall_test sysroot/usr/bin/ab sysroot/usr/bin/test sysroot/usr/bin/strace sysroot/usr/bin/forks sysroot/usr/bin/bg sysroot/usr/bin/float sysroot/usr/bin/clone sysroot/usr/bin/init sysroot/usr/bin/mmap sysroot/usr/bin/cat sysroot/usr/bin/bf sysroot/usr/bin/column sysroot/usr/bin/bomb sysroot/usr/bin/create sysroot/usr/bin/rot13 sysroot/usr/bin/trace sysroot/usr/bin/top sysroot/usr/bin/bf2 sysroot/usr/bin/insmod sysroot/usr/bin/head

include $(shell find dep -name '*.d')
