.PHONY: mp_all mp_clean mp_install

mp_all: build-x86_64/libc.a build-x86_64/libk.a build-x86_64/liblinker.a build-x86_64/ngk.elf build-x86_64/crt0.o build-x86_64/sh build-x86_64/procmod.ko build-x86_64/testmod.ko build-x86_64/thread.ko build-x86_64/validate.ko build-x86_64/bf build-x86_64/bg build-x86_64/ls build-x86_64/syscall_test build-x86_64/bomb build-x86_64/column build-x86_64/trace build-x86_64/sigtest build-x86_64/head build-x86_64/crash build-x86_64/false build-x86_64/hog build-x86_64/threadss build-x86_64/test build-x86_64/pipe build-x86_64/cat build-x86_64/threads build-x86_64/malloctest build-x86_64/init build-x86_64/insmod build-x86_64/polltest build-x86_64/kill build-x86_64/time build-x86_64/strace build-x86_64/sleep build-x86_64/forks build-x86_64/multiread build-x86_64/threadsw build-x86_64/ab build-x86_64/write build-x86_64/segv build-x86_64/echo build-x86_64/sg build-x86_64/uthread build-x86_64/top build-x86_64/busy build-x86_64/rot13 build-x86_64/create build-x86_64/args build-x86_64/fcat build-x86_64/uname build-x86_64/float build-x86_64/fio build-x86_64/clone build-x86_64/bf2 build-x86_64/what

mp_clean:
	@echo "Clean magpie objects"
	@rm -f build-x86_64/libc.a build-x86_64/libc/syscall.o build-x86_64/libc/errno.o build-x86_64/libc/vector.o build-x86_64/libc/syscalls.o build-x86_64/libc/stdlib.o build-x86_64/libc/locale.o build-x86_64/libc/fstdio.o build-x86_64/libc/malloc.o build-x86_64/libc/signal.o build-x86_64/libc/entry.o build-x86_64/libc/strtod.o build-x86_64/libc/ctype.o build-x86_64/libc/unistd.o build-x86_64/libc/stdio.o build-x86_64/libc/time.o build-x86_64/libc/string.o build-x86_64/libc/fstdio_unlocked.o build-x86_64/libc/x86_64/nightingale.o build-x86_64/libc/todo.o build-x86_64/libc/crtn.o build-x86_64/libc/crt0.o build-x86_64/libc/setjmp.o build-x86_64/libc/crti.o build-x86_64/libk.a build-x86_64/libk/ctype.o build-x86_64/libk/string.o build-x86_64/libk/stdio.o build-x86_64/libk/malloc.o build-x86_64/libk/errno.o build-x86_64/libk/signal.o build-x86_64/libk/x86_64/nightingale.o build-x86_64/libk/setjmp.o build-x86_64/liblinker.a build-x86_64/linker/elf.o build-x86_64/ngk.elf build-x86_64/kernel/pci.o build-x86_64/kernel/syscall.o build-x86_64/kernel/tty.o build-x86_64/kernel/sync_testbed.o build-x86_64/kernel/debug.o build-x86_64/kernel/mod.o build-x86_64/kernel/ringbuf.o build-x86_64/kernel/trace.o build-x86_64/kernel/tests.o build-x86_64/kernel/procfile.o build-x86_64/kernel/signal.o build-x86_64/kernel/rand.o build-x86_64/kernel/serial.o build-x86_64/kernel/ubsan.o build-x86_64/kernel/dmgr.o build-x86_64/kernel/pipe.o build-x86_64/kernel/char_devices.o build-x86_64/kernel/pmm.o build-x86_64/kernel/mman.o build-x86_64/kernel/tarfs.o build-x86_64/kernel/fs.o build-x86_64/kernel/string.o build-x86_64/kernel/spalloc.o build-x86_64/kernel/sync.o build-x86_64/kernel/irq.o build-x86_64/kernel/thread.o build-x86_64/kernel/uname.o build-x86_64/kernel/timer.o build-x86_64/kernel/membuf.o build-x86_64/kernel/multiboot.o build-x86_64/kernel/main.o build-x86_64/x86/uart.o build-x86_64/x86/vmm.o build-x86_64/x86/halt.o build-x86_64/x86/cpu.o build-x86_64/x86/pit.o build-x86_64/x86/interrupt.o build-x86_64/x86/acpi.o build-x86_64/x86/apic.o build-x86_64/x86/pic.o build-x86_64/x86/isrs.o build-x86_64/x86/boot.o build-x86_64/crt0.o build-x86_64/crt/crt0.o build-x86_64/sh build-x86_64/sh-/token.o build-x86_64/sh-/readline.o build-x86_64/sh-/sh.o build-x86_64/sh-/parse.o build-x86_64/procmod.ko build-x86_64/modules/procmod.o build-x86_64/testmod.ko build-x86_64/modules/testmod.o build-x86_64/thread.ko build-x86_64/modules/thread.o build-x86_64/validate.ko build-x86_64/modules/validate.o build-x86_64/bf build-x86_64/user/bf.o build-x86_64/bg build-x86_64/user/bg.o build-x86_64/ls build-x86_64/user/ls.o build-x86_64/syscall_test build-x86_64/user/syscall_test.o build-x86_64/bomb build-x86_64/user/bomb.o build-x86_64/column build-x86_64/user/column.o build-x86_64/trace build-x86_64/user/trace.o build-x86_64/sigtest build-x86_64/user/sigtest.o build-x86_64/head build-x86_64/user/head.o build-x86_64/crash build-x86_64/user/crash.o build-x86_64/false build-x86_64/user/false.o build-x86_64/hog build-x86_64/user/hog.o build-x86_64/threadss build-x86_64/user/threadss.o build-x86_64/test build-x86_64/user/test.o build-x86_64/pipe build-x86_64/user/pipe.o build-x86_64/cat build-x86_64/user/cat.o build-x86_64/threads build-x86_64/user/threads.o build-x86_64/malloctest build-x86_64/user/malloctest.o build-x86_64/init build-x86_64/user/init.o build-x86_64/insmod build-x86_64/user/insmod.o build-x86_64/polltest build-x86_64/user/polltest.o build-x86_64/kill build-x86_64/user/kill.o build-x86_64/time build-x86_64/user/time.o build-x86_64/strace build-x86_64/user/strace.o build-x86_64/sleep build-x86_64/user/sleep.o build-x86_64/forks build-x86_64/user/forks.o build-x86_64/multiread build-x86_64/user/multiread.o build-x86_64/threadsw build-x86_64/user/threadsw.o build-x86_64/ab build-x86_64/user/ab.o build-x86_64/write build-x86_64/user/write.o build-x86_64/segv build-x86_64/user/segv.o build-x86_64/echo build-x86_64/user/echo.o build-x86_64/sg build-x86_64/user/sg.o build-x86_64/uthread build-x86_64/user/uthread.o build-x86_64/top build-x86_64/user/top.o build-x86_64/busy build-x86_64/user/busy.o build-x86_64/rot13 build-x86_64/user/rot13.o build-x86_64/create build-x86_64/user/create.o build-x86_64/args build-x86_64/user/args.o build-x86_64/fcat build-x86_64/user/fcat.o build-x86_64/uname build-x86_64/user/uname.o build-x86_64/float build-x86_64/user/float.o build-x86_64/fio build-x86_64/user/fio.o build-x86_64/clone build-x86_64/user/clone.o build-x86_64/bf2 build-x86_64/user/bf2.o build-x86_64/what build-x86_64/user/what.o


build-x86_64/libc.a: build-x86_64/libc/syscall.o build-x86_64/libc/errno.o build-x86_64/libc/vector.o build-x86_64/libc/syscalls.o build-x86_64/libc/stdlib.o build-x86_64/libc/locale.o build-x86_64/libc/fstdio.o build-x86_64/libc/malloc.o build-x86_64/libc/signal.o build-x86_64/libc/entry.o build-x86_64/libc/strtod.o build-x86_64/libc/ctype.o build-x86_64/libc/unistd.o build-x86_64/libc/stdio.o build-x86_64/libc/time.o build-x86_64/libc/string.o build-x86_64/libc/fstdio_unlocked.o build-x86_64/libc/x86_64/nightingale.o build-x86_64/libc/todo.o build-x86_64/libc/crtn.o build-x86_64/libc/crt0.o build-x86_64/libc/setjmp.o build-x86_64/libc/crti.o 
	ar rcs -o build-x86_64/libc.a build-x86_64/libc/syscall.o build-x86_64/libc/errno.o build-x86_64/libc/vector.o build-x86_64/libc/syscalls.o build-x86_64/libc/stdlib.o build-x86_64/libc/locale.o build-x86_64/libc/fstdio.o build-x86_64/libc/malloc.o build-x86_64/libc/signal.o build-x86_64/libc/entry.o build-x86_64/libc/strtod.o build-x86_64/libc/ctype.o build-x86_64/libc/unistd.o build-x86_64/libc/stdio.o build-x86_64/libc/time.o build-x86_64/libc/string.o build-x86_64/libc/fstdio_unlocked.o build-x86_64/libc/x86_64/nightingale.o build-x86_64/libc/todo.o build-x86_64/libc/crtn.o build-x86_64/libc/crt0.o build-x86_64/libc/setjmp.o build-x86_64/libc/crti.o
build-x86_64/libc/syscall.o: libc/syscall.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/syscall.d -c libc/syscall.c -o build-x86_64/libc/syscall.o

build-x86_64/libc/errno.o: libc/errno.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/errno.d -c libc/errno.c -o build-x86_64/libc/errno.o

build-x86_64/libc/vector.o: libc/vector.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/vector.d -c libc/vector.c -o build-x86_64/libc/vector.o

build-x86_64/libc/syscalls.o: libc/syscalls.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/syscalls.d -c libc/syscalls.c -o build-x86_64/libc/syscalls.o

build-x86_64/libc/stdlib.o: libc/stdlib.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/stdlib.d -c libc/stdlib.c -o build-x86_64/libc/stdlib.o

build-x86_64/libc/locale.o: libc/locale.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/locale.d -c libc/locale.c -o build-x86_64/libc/locale.o

build-x86_64/libc/fstdio.o: libc/fstdio.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/fstdio.d -c libc/fstdio.c -o build-x86_64/libc/fstdio.o

build-x86_64/libc/malloc.o: libc/malloc.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/malloc.d -c libc/malloc.c -o build-x86_64/libc/malloc.o

build-x86_64/libc/signal.o: libc/signal.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/signal.d -c libc/signal.c -o build-x86_64/libc/signal.o

build-x86_64/libc/entry.o: libc/entry.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/entry.d -c libc/entry.c -o build-x86_64/libc/entry.o

build-x86_64/libc/strtod.o: libc/strtod.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/strtod.d -c libc/strtod.c -o build-x86_64/libc/strtod.o

build-x86_64/libc/ctype.o: libc/ctype.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/ctype.d -c libc/ctype.c -o build-x86_64/libc/ctype.o

build-x86_64/libc/unistd.o: libc/unistd.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/unistd.d -c libc/unistd.c -o build-x86_64/libc/unistd.o

build-x86_64/libc/stdio.o: libc/stdio.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/stdio.d -c libc/stdio.c -o build-x86_64/libc/stdio.o

build-x86_64/libc/time.o: libc/time.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/time.d -c libc/time.c -o build-x86_64/libc/time.o

build-x86_64/libc/string.o: libc/string.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/string.d -c libc/string.c -o build-x86_64/libc/string.o

build-x86_64/libc/fstdio_unlocked.o: libc/fstdio_unlocked.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/fstdio_unlocked.d -c libc/fstdio_unlocked.c -o build-x86_64/libc/fstdio_unlocked.o

build-x86_64/libc/x86_64/nightingale.o: libc/x86_64/nightingale.c
	@mkdir -p build-x86_64/libc/x86_64
	@mkdir -p dep/libc/x86_64
	x86_64-nightingale-gcc  -MD -MF dep/libc/x86_64/nightingale.d -c libc/x86_64/nightingale.c -o build-x86_64/libc/x86_64/nightingale.o

build-x86_64/libc/todo.o: libc/todo.c
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	x86_64-nightingale-gcc  -MD -MF dep/libc/todo.d -c libc/todo.c -o build-x86_64/libc/todo.o

build-x86_64/libc/crtn.o: libc/crtn.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	gcc  -MD -MF dep/libc/crtn.d -c libc/crtn.S -o build-x86_64/libc/crtn.o

build-x86_64/libc/crt0.o: libc/crt0.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	gcc  -MD -MF dep/libc/crt0.d -c libc/crt0.S -o build-x86_64/libc/crt0.o

build-x86_64/libc/setjmp.o: libc/setjmp.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	gcc  -MD -MF dep/libc/setjmp.d -c libc/setjmp.S -o build-x86_64/libc/setjmp.o

build-x86_64/libc/crti.o: libc/crti.S
	@mkdir -p build-x86_64/libc
	@mkdir -p dep/libc
	gcc  -MD -MF dep/libc/crti.d -c libc/crti.S -o build-x86_64/libc/crti.o

build-x86_64/libk.a: build-x86_64/libk/ctype.o build-x86_64/libk/string.o build-x86_64/libk/stdio.o build-x86_64/libk/malloc.o build-x86_64/libk/errno.o build-x86_64/libk/signal.o build-x86_64/libk/x86_64/nightingale.o build-x86_64/libk/setjmp.o 
	ar rcs -o build-x86_64/libk.a build-x86_64/libk/ctype.o build-x86_64/libk/string.o build-x86_64/libk/stdio.o build-x86_64/libk/malloc.o build-x86_64/libk/errno.o build-x86_64/libk/signal.o build-x86_64/libk/x86_64/nightingale.o build-x86_64/libk/setjmp.o
build-x86_64/libk/ctype.o: libc/ctype.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/ctype.d -c libc/ctype.c -o build-x86_64/libk/ctype.o

build-x86_64/libk/string.o: libc/string.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/string.d -c libc/string.c -o build-x86_64/libk/string.o

build-x86_64/libk/stdio.o: libc/stdio.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/stdio.d -c libc/stdio.c -o build-x86_64/libk/stdio.o

build-x86_64/libk/malloc.o: libc/malloc.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/malloc.d -c libc/malloc.c -o build-x86_64/libk/malloc.o

build-x86_64/libk/errno.o: libc/errno.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/errno.d -c libc/errno.c -o build-x86_64/libk/errno.o

build-x86_64/libk/signal.o: libc/signal.c
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/signal.d -c libc/signal.c -o build-x86_64/libk/signal.o

build-x86_64/libk/x86_64/nightingale.o: libc/x86_64/nightingale.c
	@mkdir -p build-x86_64/libk/x86_64
	@mkdir -p dep/libk/x86_64
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/x86_64/nightingale.d -c libc/x86_64/nightingale.c -o build-x86_64/libk/x86_64/nightingale.o

build-x86_64/libk/setjmp.o: libc/setjmp.S
	@mkdir -p build-x86_64/libk
	@mkdir -p dep/libk
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/libk/setjmp.d -c libc/setjmp.S -o build-x86_64/libk/setjmp.o

build-x86_64/liblinker.a: build-x86_64/linker/elf.o 
	ar rcs -o build-x86_64/liblinker.a build-x86_64/linker/elf.o
build-x86_64/linker/elf.o: linker/elf.c
	@mkdir -p build-x86_64/linker
	@mkdir -p dep/linker
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/linker/elf.d -c linker/elf.c -o build-x86_64/linker/elf.o

build-x86_64/ngk.elf: build-x86_64/kernel/pci.o build-x86_64/kernel/syscall.o build-x86_64/kernel/tty.o build-x86_64/kernel/sync_testbed.o build-x86_64/kernel/debug.o build-x86_64/kernel/mod.o build-x86_64/kernel/ringbuf.o build-x86_64/kernel/trace.o build-x86_64/kernel/tests.o build-x86_64/kernel/procfile.o build-x86_64/kernel/signal.o build-x86_64/kernel/rand.o build-x86_64/kernel/serial.o build-x86_64/kernel/ubsan.o build-x86_64/kernel/dmgr.o build-x86_64/kernel/pipe.o build-x86_64/kernel/char_devices.o build-x86_64/kernel/pmm.o build-x86_64/kernel/mman.o build-x86_64/kernel/tarfs.o build-x86_64/kernel/fs.o build-x86_64/kernel/string.o build-x86_64/kernel/spalloc.o build-x86_64/kernel/sync.o build-x86_64/kernel/irq.o build-x86_64/kernel/thread.o build-x86_64/kernel/uname.o build-x86_64/kernel/timer.o build-x86_64/kernel/membuf.o build-x86_64/kernel/multiboot.o build-x86_64/kernel/main.o build-x86_64/x86/uart.o build-x86_64/x86/vmm.o build-x86_64/x86/halt.o build-x86_64/x86/cpu.o build-x86_64/x86/pit.o build-x86_64/x86/interrupt.o build-x86_64/x86/acpi.o build-x86_64/x86/apic.o build-x86_64/x86/pic.o build-x86_64/x86/isrs.o build-x86_64/x86/boot.o build-x86_64/libk.a build-x86_64/liblinker.a
	x86_64-nightingale-gcc -nostdlib -Tx86/link_hh.ld -zmax-page-size=0x1000 -g -Lbuild-x86_64 -o build-x86_64/ngk.elf build-x86_64/kernel/pci.o build-x86_64/kernel/syscall.o build-x86_64/kernel/tty.o build-x86_64/kernel/sync_testbed.o build-x86_64/kernel/debug.o build-x86_64/kernel/mod.o build-x86_64/kernel/ringbuf.o build-x86_64/kernel/trace.o build-x86_64/kernel/tests.o build-x86_64/kernel/procfile.o build-x86_64/kernel/signal.o build-x86_64/kernel/rand.o build-x86_64/kernel/serial.o build-x86_64/kernel/ubsan.o build-x86_64/kernel/dmgr.o build-x86_64/kernel/pipe.o build-x86_64/kernel/char_devices.o build-x86_64/kernel/pmm.o build-x86_64/kernel/mman.o build-x86_64/kernel/tarfs.o build-x86_64/kernel/fs.o build-x86_64/kernel/string.o build-x86_64/kernel/spalloc.o build-x86_64/kernel/sync.o build-x86_64/kernel/irq.o build-x86_64/kernel/thread.o build-x86_64/kernel/uname.o build-x86_64/kernel/timer.o build-x86_64/kernel/membuf.o build-x86_64/kernel/multiboot.o build-x86_64/kernel/main.o build-x86_64/x86/uart.o build-x86_64/x86/vmm.o build-x86_64/x86/halt.o build-x86_64/x86/cpu.o build-x86_64/x86/pit.o build-x86_64/x86/interrupt.o build-x86_64/x86/acpi.o build-x86_64/x86/apic.o build-x86_64/x86/pic.o build-x86_64/x86/isrs.o build-x86_64/x86/boot.o -lk -llinker -lgcc
build-x86_64/kernel/pci.o: kernel/pci.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/pci.d -c kernel/pci.c -o build-x86_64/kernel/pci.o

build-x86_64/kernel/syscall.o: kernel/syscall.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/syscall.d -c kernel/syscall.c -o build-x86_64/kernel/syscall.o

build-x86_64/kernel/tty.o: kernel/tty.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/tty.d -c kernel/tty.c -o build-x86_64/kernel/tty.o

build-x86_64/kernel/sync_testbed.o: kernel/sync_testbed.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/sync_testbed.d -c kernel/sync_testbed.c -o build-x86_64/kernel/sync_testbed.o

build-x86_64/kernel/debug.o: kernel/debug.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/debug.d -c kernel/debug.c -o build-x86_64/kernel/debug.o

build-x86_64/kernel/mod.o: kernel/mod.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/mod.d -c kernel/mod.c -o build-x86_64/kernel/mod.o

build-x86_64/kernel/ringbuf.o: kernel/ringbuf.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/ringbuf.d -c kernel/ringbuf.c -o build-x86_64/kernel/ringbuf.o

build-x86_64/kernel/trace.o: kernel/trace.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/trace.d -c kernel/trace.c -o build-x86_64/kernel/trace.o

build-x86_64/kernel/tests.o: kernel/tests.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/tests.d -c kernel/tests.c -o build-x86_64/kernel/tests.o

build-x86_64/kernel/procfile.o: kernel/procfile.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/procfile.d -c kernel/procfile.c -o build-x86_64/kernel/procfile.o

build-x86_64/kernel/signal.o: kernel/signal.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/signal.d -c kernel/signal.c -o build-x86_64/kernel/signal.o

build-x86_64/kernel/rand.o: kernel/rand.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/rand.d -c kernel/rand.c -o build-x86_64/kernel/rand.o

build-x86_64/kernel/serial.o: kernel/serial.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/serial.d -c kernel/serial.c -o build-x86_64/kernel/serial.o

build-x86_64/kernel/ubsan.o: kernel/ubsan.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/ubsan.d -c kernel/ubsan.c -o build-x86_64/kernel/ubsan.o

build-x86_64/kernel/dmgr.o: kernel/dmgr.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/dmgr.d -c kernel/dmgr.c -o build-x86_64/kernel/dmgr.o

build-x86_64/kernel/pipe.o: kernel/pipe.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/pipe.d -c kernel/pipe.c -o build-x86_64/kernel/pipe.o

build-x86_64/kernel/char_devices.o: kernel/char_devices.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/char_devices.d -c kernel/char_devices.c -o build-x86_64/kernel/char_devices.o

build-x86_64/kernel/pmm.o: kernel/pmm.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/pmm.d -c kernel/pmm.c -o build-x86_64/kernel/pmm.o

build-x86_64/kernel/mman.o: kernel/mman.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/mman.d -c kernel/mman.c -o build-x86_64/kernel/mman.o

build-x86_64/kernel/tarfs.o: kernel/tarfs.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/tarfs.d -c kernel/tarfs.c -o build-x86_64/kernel/tarfs.o

build-x86_64/kernel/fs.o: kernel/fs.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/fs.d -c kernel/fs.c -o build-x86_64/kernel/fs.o

build-x86_64/kernel/string.o: kernel/string.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/string.d -c kernel/string.c -o build-x86_64/kernel/string.o

build-x86_64/kernel/spalloc.o: kernel/spalloc.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/spalloc.d -c kernel/spalloc.c -o build-x86_64/kernel/spalloc.o

build-x86_64/kernel/sync.o: kernel/sync.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/sync.d -c kernel/sync.c -o build-x86_64/kernel/sync.o

build-x86_64/kernel/irq.o: kernel/irq.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/irq.d -c kernel/irq.c -o build-x86_64/kernel/irq.o

build-x86_64/kernel/thread.o: kernel/thread.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/thread.d -c kernel/thread.c -o build-x86_64/kernel/thread.o

build-x86_64/kernel/uname.o: kernel/uname.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/uname.d -c kernel/uname.c -o build-x86_64/kernel/uname.o

build-x86_64/kernel/timer.o: kernel/timer.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/timer.d -c kernel/timer.c -o build-x86_64/kernel/timer.o

build-x86_64/kernel/membuf.o: kernel/membuf.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/membuf.d -c kernel/membuf.c -o build-x86_64/kernel/membuf.o

build-x86_64/kernel/multiboot.o: kernel/multiboot.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/multiboot.d -c kernel/multiboot.c -o build-x86_64/kernel/multiboot.o

build-x86_64/kernel/main.o: kernel/main.c
	@mkdir -p build-x86_64/kernel
	@mkdir -p dep/kernel
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/kernel/main.d -c kernel/main.c -o build-x86_64/kernel/main.o

build-x86_64/x86/uart.o: x86/uart.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/uart.d -c x86/uart.c -o build-x86_64/x86/uart.o

build-x86_64/x86/vmm.o: x86/vmm.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/vmm.d -c x86/vmm.c -o build-x86_64/x86/vmm.o

build-x86_64/x86/halt.o: x86/halt.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/halt.d -c x86/halt.c -o build-x86_64/x86/halt.o

build-x86_64/x86/cpu.o: x86/cpu.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/cpu.d -c x86/cpu.c -o build-x86_64/x86/cpu.o

build-x86_64/x86/pit.o: x86/pit.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/pit.d -c x86/pit.c -o build-x86_64/x86/pit.o

build-x86_64/x86/interrupt.o: x86/interrupt.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/interrupt.d -c x86/interrupt.c -o build-x86_64/x86/interrupt.o

build-x86_64/x86/acpi.o: x86/acpi.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/acpi.d -c x86/acpi.c -o build-x86_64/x86/acpi.o

build-x86_64/x86/apic.o: x86/apic.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/apic.d -c x86/apic.c -o build-x86_64/x86/apic.o

build-x86_64/x86/pic.o: x86/pic.c
	@mkdir -p build-x86_64/x86
	@mkdir -p dep/x86
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/x86/pic.d -c x86/pic.c -o build-x86_64/x86/pic.o

build-x86_64/x86/isrs.o: x86/isrs.asm
	@mkdir -p build-x86_64/x86
	nasm -felf64 -o build-x86_64/x86/isrs.o x86/isrs.asm

build-x86_64/x86/boot.o: x86/boot.asm
	@mkdir -p build-x86_64/x86
	nasm -felf64 -o build-x86_64/x86/boot.o x86/boot.asm

build-x86_64/crt0.o: build-x86_64/crt/crt0.o
	@cp --preserve=timestamps build-x86_64/crt/crt0.o build-x86_64/crt0.o
build-x86_64/crt/crt0.o: libc/crt0.S
	@mkdir -p build-x86_64/crt
	@mkdir -p dep/crt
	x86_64-nightingale-gcc  -MD -MF dep/crt/crt0.d -c libc/crt0.S -o build-x86_64/crt/crt0.o

build-x86_64/sh: build-x86_64/sh-/token.o build-x86_64/sh-/readline.o build-x86_64/sh-/sh.o build-x86_64/sh-/parse.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/sh build-x86_64/sh-/token.o build-x86_64/sh-/readline.o build-x86_64/sh-/sh.o build-x86_64/sh-/parse.o 
build-x86_64/sh-/token.o: sh/token.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/sh-/token.d -c sh/token.c -o build-x86_64/sh-/token.o

build-x86_64/sh-/readline.o: sh/readline.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/sh-/readline.d -c sh/readline.c -o build-x86_64/sh-/readline.o

build-x86_64/sh-/sh.o: sh/sh.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/sh-/sh.d -c sh/sh.c -o build-x86_64/sh-/sh.o

build-x86_64/sh-/parse.o: sh/parse.c
	@mkdir -p build-x86_64/sh-
	@mkdir -p dep/sh-
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/sh-/parse.d -c sh/parse.c -o build-x86_64/sh-/parse.o

build-x86_64/procmod.ko: build-x86_64/modules/procmod.o
	@cp --preserve=timestamps build-x86_64/modules/procmod.o build-x86_64/procmod.ko
build-x86_64/modules/procmod.o: modules/procmod.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/modules/procmod.d -c modules/procmod.c -o build-x86_64/modules/procmod.o

build-x86_64/testmod.ko: build-x86_64/modules/testmod.o
	@cp --preserve=timestamps build-x86_64/modules/testmod.o build-x86_64/testmod.ko
build-x86_64/modules/testmod.o: modules/testmod.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/modules/testmod.d -c modules/testmod.c -o build-x86_64/modules/testmod.o

build-x86_64/thread.ko: build-x86_64/modules/thread.o
	@cp --preserve=timestamps build-x86_64/modules/thread.o build-x86_64/thread.ko
build-x86_64/modules/thread.o: modules/thread.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/modules/thread.d -c modules/thread.c -o build-x86_64/modules/thread.o

build-x86_64/validate.ko: build-x86_64/modules/validate.o
	@cp --preserve=timestamps build-x86_64/modules/validate.o build-x86_64/validate.ko
build-x86_64/modules/validate.o: modules/validate.c
	@mkdir -p build-x86_64/modules
	@mkdir -p dep/modules
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -ffreestanding -mno-red-zone -mno-80387 -mno-mmx -mno-sse -mno-sse2 -nostdlib -fno-asynchronous-unwind-tables -fno-omit-frame-pointer -DNIGHTINGALE_VERSION=\"v0.10.4-54-g44478ec\" -D__kernel__=1 -D_NG=1 -mcmodel=kernel -MD -MF dep/modules/validate.d -c modules/validate.c -o build-x86_64/modules/validate.o

build-x86_64/bf: build-x86_64/user/bf.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/bf build-x86_64/user/bf.o 
build-x86_64/user/bf.o: user/bf.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/bf.d -c user/bf.c -o build-x86_64/user/bf.o

build-x86_64/bg: build-x86_64/user/bg.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/bg build-x86_64/user/bg.o 
build-x86_64/user/bg.o: user/bg.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/bg.d -c user/bg.c -o build-x86_64/user/bg.o

build-x86_64/ls: build-x86_64/user/ls.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/ls build-x86_64/user/ls.o 
build-x86_64/user/ls.o: user/ls.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/ls.d -c user/ls.c -o build-x86_64/user/ls.o

build-x86_64/syscall_test: build-x86_64/user/syscall_test.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/syscall_test build-x86_64/user/syscall_test.o 
build-x86_64/user/syscall_test.o: user/syscall_test.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/syscall_test.d -c user/syscall_test.c -o build-x86_64/user/syscall_test.o

build-x86_64/bomb: build-x86_64/user/bomb.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/bomb build-x86_64/user/bomb.o 
build-x86_64/user/bomb.o: user/bomb.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/bomb.d -c user/bomb.c -o build-x86_64/user/bomb.o

build-x86_64/column: build-x86_64/user/column.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/column build-x86_64/user/column.o 
build-x86_64/user/column.o: user/column.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/column.d -c user/column.c -o build-x86_64/user/column.o

build-x86_64/trace: build-x86_64/user/trace.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/trace build-x86_64/user/trace.o 
build-x86_64/user/trace.o: user/trace.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/trace.d -c user/trace.c -o build-x86_64/user/trace.o

build-x86_64/sigtest: build-x86_64/user/sigtest.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/sigtest build-x86_64/user/sigtest.o 
build-x86_64/user/sigtest.o: user/sigtest.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/sigtest.d -c user/sigtest.c -o build-x86_64/user/sigtest.o

build-x86_64/head: build-x86_64/user/head.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/head build-x86_64/user/head.o 
build-x86_64/user/head.o: user/head.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/head.d -c user/head.c -o build-x86_64/user/head.o

build-x86_64/crash: build-x86_64/user/crash.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/crash build-x86_64/user/crash.o 
build-x86_64/user/crash.o: user/crash.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/crash.d -c user/crash.c -o build-x86_64/user/crash.o

build-x86_64/false: build-x86_64/user/false.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/false build-x86_64/user/false.o 
build-x86_64/user/false.o: user/false.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/false.d -c user/false.c -o build-x86_64/user/false.o

build-x86_64/hog: build-x86_64/user/hog.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/hog build-x86_64/user/hog.o 
build-x86_64/user/hog.o: user/hog.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/hog.d -c user/hog.c -o build-x86_64/user/hog.o

build-x86_64/threadss: build-x86_64/user/threadss.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/threadss build-x86_64/user/threadss.o 
build-x86_64/user/threadss.o: user/threadss.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/threadss.d -c user/threadss.c -o build-x86_64/user/threadss.o

build-x86_64/test: build-x86_64/user/test.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/test build-x86_64/user/test.o 
build-x86_64/user/test.o: user/test.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/test.d -c user/test.c -o build-x86_64/user/test.o

build-x86_64/pipe: build-x86_64/user/pipe.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/pipe build-x86_64/user/pipe.o 
build-x86_64/user/pipe.o: user/pipe.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/pipe.d -c user/pipe.c -o build-x86_64/user/pipe.o

build-x86_64/cat: build-x86_64/user/cat.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/cat build-x86_64/user/cat.o 
build-x86_64/user/cat.o: user/cat.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/cat.d -c user/cat.c -o build-x86_64/user/cat.o

build-x86_64/threads: build-x86_64/user/threads.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/threads build-x86_64/user/threads.o 
build-x86_64/user/threads.o: user/threads.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/threads.d -c user/threads.c -o build-x86_64/user/threads.o

build-x86_64/malloctest: build-x86_64/user/malloctest.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/malloctest build-x86_64/user/malloctest.o 
build-x86_64/user/malloctest.o: user/malloctest.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/malloctest.d -c user/malloctest.c -o build-x86_64/user/malloctest.o

build-x86_64/init: build-x86_64/user/init.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/init build-x86_64/user/init.o 
build-x86_64/user/init.o: user/init.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/init.d -c user/init.c -o build-x86_64/user/init.o

build-x86_64/insmod: build-x86_64/user/insmod.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/insmod build-x86_64/user/insmod.o 
build-x86_64/user/insmod.o: user/insmod.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/insmod.d -c user/insmod.c -o build-x86_64/user/insmod.o

build-x86_64/polltest: build-x86_64/user/polltest.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/polltest build-x86_64/user/polltest.o 
build-x86_64/user/polltest.o: user/polltest.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/polltest.d -c user/polltest.c -o build-x86_64/user/polltest.o

build-x86_64/kill: build-x86_64/user/kill.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/kill build-x86_64/user/kill.o 
build-x86_64/user/kill.o: user/kill.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/kill.d -c user/kill.c -o build-x86_64/user/kill.o

build-x86_64/time: build-x86_64/user/time.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/time build-x86_64/user/time.o 
build-x86_64/user/time.o: user/time.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/time.d -c user/time.c -o build-x86_64/user/time.o

build-x86_64/strace: build-x86_64/user/strace.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/strace build-x86_64/user/strace.o 
build-x86_64/user/strace.o: user/strace.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/strace.d -c user/strace.c -o build-x86_64/user/strace.o

build-x86_64/sleep: build-x86_64/user/sleep.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/sleep build-x86_64/user/sleep.o 
build-x86_64/user/sleep.o: user/sleep.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/sleep.d -c user/sleep.c -o build-x86_64/user/sleep.o

build-x86_64/forks: build-x86_64/user/forks.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/forks build-x86_64/user/forks.o 
build-x86_64/user/forks.o: user/forks.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/forks.d -c user/forks.c -o build-x86_64/user/forks.o

build-x86_64/multiread: build-x86_64/user/multiread.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/multiread build-x86_64/user/multiread.o 
build-x86_64/user/multiread.o: user/multiread.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/multiread.d -c user/multiread.c -o build-x86_64/user/multiread.o

build-x86_64/threadsw: build-x86_64/user/threadsw.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/threadsw build-x86_64/user/threadsw.o 
build-x86_64/user/threadsw.o: user/threadsw.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/threadsw.d -c user/threadsw.c -o build-x86_64/user/threadsw.o

build-x86_64/ab: build-x86_64/user/ab.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/ab build-x86_64/user/ab.o 
build-x86_64/user/ab.o: user/ab.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/ab.d -c user/ab.c -o build-x86_64/user/ab.o

build-x86_64/write: build-x86_64/user/write.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/write build-x86_64/user/write.o 
build-x86_64/user/write.o: user/write.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/write.d -c user/write.c -o build-x86_64/user/write.o

build-x86_64/segv: build-x86_64/user/segv.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/segv build-x86_64/user/segv.o 
build-x86_64/user/segv.o: user/segv.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/segv.d -c user/segv.c -o build-x86_64/user/segv.o

build-x86_64/echo: build-x86_64/user/echo.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/echo build-x86_64/user/echo.o 
build-x86_64/user/echo.o: user/echo.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/echo.d -c user/echo.c -o build-x86_64/user/echo.o

build-x86_64/sg: build-x86_64/user/sg.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/sg build-x86_64/user/sg.o 
build-x86_64/user/sg.o: user/sg.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/sg.d -c user/sg.c -o build-x86_64/user/sg.o

build-x86_64/uthread: build-x86_64/user/uthread.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/uthread build-x86_64/user/uthread.o 
build-x86_64/user/uthread.o: user/uthread.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/uthread.d -c user/uthread.c -o build-x86_64/user/uthread.o

build-x86_64/top: build-x86_64/user/top.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/top build-x86_64/user/top.o 
build-x86_64/user/top.o: user/top.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/top.d -c user/top.c -o build-x86_64/user/top.o

build-x86_64/busy: build-x86_64/user/busy.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/busy build-x86_64/user/busy.o 
build-x86_64/user/busy.o: user/busy.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/busy.d -c user/busy.c -o build-x86_64/user/busy.o

build-x86_64/rot13: build-x86_64/user/rot13.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/rot13 build-x86_64/user/rot13.o 
build-x86_64/user/rot13.o: user/rot13.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/rot13.d -c user/rot13.c -o build-x86_64/user/rot13.o

build-x86_64/create: build-x86_64/user/create.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/create build-x86_64/user/create.o 
build-x86_64/user/create.o: user/create.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/create.d -c user/create.c -o build-x86_64/user/create.o

build-x86_64/args: build-x86_64/user/args.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/args build-x86_64/user/args.o 
build-x86_64/user/args.o: user/args.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/args.d -c user/args.c -o build-x86_64/user/args.o

build-x86_64/fcat: build-x86_64/user/fcat.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/fcat build-x86_64/user/fcat.o 
build-x86_64/user/fcat.o: user/fcat.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/fcat.d -c user/fcat.c -o build-x86_64/user/fcat.o

build-x86_64/uname: build-x86_64/user/uname.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/uname build-x86_64/user/uname.o 
build-x86_64/user/uname.o: user/uname.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/uname.d -c user/uname.c -o build-x86_64/user/uname.o

build-x86_64/float: build-x86_64/user/float.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/float build-x86_64/user/float.o 
build-x86_64/user/float.o: user/float.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/float.d -c user/float.c -o build-x86_64/user/float.o

build-x86_64/fio: build-x86_64/user/fio.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/fio build-x86_64/user/fio.o 
build-x86_64/user/fio.o: user/fio.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/fio.d -c user/fio.c -o build-x86_64/user/fio.o

build-x86_64/clone: build-x86_64/user/clone.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/clone build-x86_64/user/clone.o 
build-x86_64/user/clone.o: user/clone.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/clone.d -c user/clone.c -o build-x86_64/user/clone.o

build-x86_64/bf2: build-x86_64/user/bf2.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/bf2 build-x86_64/user/bf2.o 
build-x86_64/user/bf2.o: user/bf2.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/bf2.d -c user/bf2.c -o build-x86_64/user/bf2.o

build-x86_64/what: build-x86_64/user/what.o sysroot/usr/lib/libc.a sysroot/usr/lib/crt0.o
	x86_64-nightingale-gcc -static -o build-x86_64/what build-x86_64/user/what.o 
build-x86_64/user/what.o: user/what.c
	@mkdir -p build-x86_64/user
	@mkdir -p dep/user
	x86_64-nightingale-gcc -std=c11 -Wall -Wextra -Werror -pedantic -g -Og -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -Wno-address-of-packed-member -Wno-builtin-declaration-mismatch -static -MD -MF dep/user/what.d -c user/what.c -o build-x86_64/user/what.o


sysroot/usr/lib/libc.a: build-x86_64/libc.a
	@echo "install libc.a"
	@cp build-x86_64/libc.a sysroot/usr/lib/libc.a

ngk.elf: build-x86_64/ngk.elf
	@echo "install ngk.elf"
	@cp build-x86_64/ngk.elf ngk.elf

sysroot/usr/lib/crt0.o: build-x86_64/crt0.o
	@echo "install crt0.o"
	@cp build-x86_64/crt0.o sysroot/usr/lib/crt0.o

sysroot/usr/bin/sh: build-x86_64/sh
	@echo "install sh"
	@cp build-x86_64/sh sysroot/usr/bin/sh

sysroot/usr/bin/procmod.ko: build-x86_64/procmod.ko
	@echo "install procmod.ko"
	@cp build-x86_64/procmod.ko sysroot/usr/bin/procmod.ko

sysroot/usr/bin/testmod.ko: build-x86_64/testmod.ko
	@echo "install testmod.ko"
	@cp build-x86_64/testmod.ko sysroot/usr/bin/testmod.ko

sysroot/usr/bin/thread.ko: build-x86_64/thread.ko
	@echo "install thread.ko"
	@cp build-x86_64/thread.ko sysroot/usr/bin/thread.ko

sysroot/usr/bin/validate.ko: build-x86_64/validate.ko
	@echo "install validate.ko"
	@cp build-x86_64/validate.ko sysroot/usr/bin/validate.ko

sysroot/usr/bin/bf: build-x86_64/bf
	@echo "install bf"
	@cp build-x86_64/bf sysroot/usr/bin/bf

sysroot/usr/bin/bg: build-x86_64/bg
	@echo "install bg"
	@cp build-x86_64/bg sysroot/usr/bin/bg

sysroot/usr/bin/ls: build-x86_64/ls
	@echo "install ls"
	@cp build-x86_64/ls sysroot/usr/bin/ls

sysroot/usr/bin/syscall_test: build-x86_64/syscall_test
	@echo "install syscall_test"
	@cp build-x86_64/syscall_test sysroot/usr/bin/syscall_test

sysroot/usr/bin/bomb: build-x86_64/bomb
	@echo "install bomb"
	@cp build-x86_64/bomb sysroot/usr/bin/bomb

sysroot/usr/bin/column: build-x86_64/column
	@echo "install column"
	@cp build-x86_64/column sysroot/usr/bin/column

sysroot/usr/bin/trace: build-x86_64/trace
	@echo "install trace"
	@cp build-x86_64/trace sysroot/usr/bin/trace

sysroot/usr/bin/sigtest: build-x86_64/sigtest
	@echo "install sigtest"
	@cp build-x86_64/sigtest sysroot/usr/bin/sigtest

sysroot/usr/bin/head: build-x86_64/head
	@echo "install head"
	@cp build-x86_64/head sysroot/usr/bin/head

sysroot/usr/bin/crash: build-x86_64/crash
	@echo "install crash"
	@cp build-x86_64/crash sysroot/usr/bin/crash

sysroot/usr/bin/false: build-x86_64/false
	@echo "install false"
	@cp build-x86_64/false sysroot/usr/bin/false

sysroot/usr/bin/hog: build-x86_64/hog
	@echo "install hog"
	@cp build-x86_64/hog sysroot/usr/bin/hog

sysroot/usr/bin/threadss: build-x86_64/threadss
	@echo "install threadss"
	@cp build-x86_64/threadss sysroot/usr/bin/threadss

sysroot/usr/bin/test: build-x86_64/test
	@echo "install test"
	@cp build-x86_64/test sysroot/usr/bin/test

sysroot/usr/bin/pipe: build-x86_64/pipe
	@echo "install pipe"
	@cp build-x86_64/pipe sysroot/usr/bin/pipe

sysroot/usr/bin/cat: build-x86_64/cat
	@echo "install cat"
	@cp build-x86_64/cat sysroot/usr/bin/cat

sysroot/usr/bin/threads: build-x86_64/threads
	@echo "install threads"
	@cp build-x86_64/threads sysroot/usr/bin/threads

sysroot/usr/bin/malloctest: build-x86_64/malloctest
	@echo "install malloctest"
	@cp build-x86_64/malloctest sysroot/usr/bin/malloctest

sysroot/usr/bin/init: build-x86_64/init
	@echo "install init"
	@cp build-x86_64/init sysroot/usr/bin/init

sysroot/usr/bin/insmod: build-x86_64/insmod
	@echo "install insmod"
	@cp build-x86_64/insmod sysroot/usr/bin/insmod

sysroot/usr/bin/polltest: build-x86_64/polltest
	@echo "install polltest"
	@cp build-x86_64/polltest sysroot/usr/bin/polltest

sysroot/usr/bin/kill: build-x86_64/kill
	@echo "install kill"
	@cp build-x86_64/kill sysroot/usr/bin/kill

sysroot/usr/bin/time: build-x86_64/time
	@echo "install time"
	@cp build-x86_64/time sysroot/usr/bin/time

sysroot/usr/bin/strace: build-x86_64/strace
	@echo "install strace"
	@cp build-x86_64/strace sysroot/usr/bin/strace

sysroot/usr/bin/sleep: build-x86_64/sleep
	@echo "install sleep"
	@cp build-x86_64/sleep sysroot/usr/bin/sleep

sysroot/usr/bin/forks: build-x86_64/forks
	@echo "install forks"
	@cp build-x86_64/forks sysroot/usr/bin/forks

sysroot/usr/bin/multiread: build-x86_64/multiread
	@echo "install multiread"
	@cp build-x86_64/multiread sysroot/usr/bin/multiread

sysroot/usr/bin/threadsw: build-x86_64/threadsw
	@echo "install threadsw"
	@cp build-x86_64/threadsw sysroot/usr/bin/threadsw

sysroot/usr/bin/ab: build-x86_64/ab
	@echo "install ab"
	@cp build-x86_64/ab sysroot/usr/bin/ab

sysroot/usr/bin/write: build-x86_64/write
	@echo "install write"
	@cp build-x86_64/write sysroot/usr/bin/write

sysroot/usr/bin/segv: build-x86_64/segv
	@echo "install segv"
	@cp build-x86_64/segv sysroot/usr/bin/segv

sysroot/usr/bin/echo: build-x86_64/echo
	@echo "install echo"
	@cp build-x86_64/echo sysroot/usr/bin/echo

sysroot/usr/bin/sg: build-x86_64/sg
	@echo "install sg"
	@cp build-x86_64/sg sysroot/usr/bin/sg

sysroot/usr/bin/uthread: build-x86_64/uthread
	@echo "install uthread"
	@cp build-x86_64/uthread sysroot/usr/bin/uthread

sysroot/usr/bin/top: build-x86_64/top
	@echo "install top"
	@cp build-x86_64/top sysroot/usr/bin/top

sysroot/usr/bin/busy: build-x86_64/busy
	@echo "install busy"
	@cp build-x86_64/busy sysroot/usr/bin/busy

sysroot/usr/bin/rot13: build-x86_64/rot13
	@echo "install rot13"
	@cp build-x86_64/rot13 sysroot/usr/bin/rot13

sysroot/usr/bin/create: build-x86_64/create
	@echo "install create"
	@cp build-x86_64/create sysroot/usr/bin/create

sysroot/usr/bin/args: build-x86_64/args
	@echo "install args"
	@cp build-x86_64/args sysroot/usr/bin/args

sysroot/usr/bin/fcat: build-x86_64/fcat
	@echo "install fcat"
	@cp build-x86_64/fcat sysroot/usr/bin/fcat

sysroot/usr/bin/uname: build-x86_64/uname
	@echo "install uname"
	@cp build-x86_64/uname sysroot/usr/bin/uname

sysroot/usr/bin/float: build-x86_64/float
	@echo "install float"
	@cp build-x86_64/float sysroot/usr/bin/float

sysroot/usr/bin/fio: build-x86_64/fio
	@echo "install fio"
	@cp build-x86_64/fio sysroot/usr/bin/fio

sysroot/usr/bin/clone: build-x86_64/clone
	@echo "install clone"
	@cp build-x86_64/clone sysroot/usr/bin/clone

sysroot/usr/bin/bf2: build-x86_64/bf2
	@echo "install bf2"
	@cp build-x86_64/bf2 sysroot/usr/bin/bf2

sysroot/usr/bin/what: build-x86_64/what
	@echo "install what"
	@cp build-x86_64/what sysroot/usr/bin/what


mp_install: sysroot/usr/lib/libc.a build-x86_64/libk.a build-x86_64/liblinker.a ngk.elf sysroot/usr/lib/crt0.o sysroot/usr/bin/sh sysroot/usr/bin/procmod.ko sysroot/usr/bin/testmod.ko sysroot/usr/bin/thread.ko sysroot/usr/bin/validate.ko sysroot/usr/bin/bf sysroot/usr/bin/bg sysroot/usr/bin/ls sysroot/usr/bin/syscall_test sysroot/usr/bin/bomb sysroot/usr/bin/column sysroot/usr/bin/trace sysroot/usr/bin/sigtest sysroot/usr/bin/head sysroot/usr/bin/crash sysroot/usr/bin/false sysroot/usr/bin/hog sysroot/usr/bin/threadss sysroot/usr/bin/test sysroot/usr/bin/pipe sysroot/usr/bin/cat sysroot/usr/bin/threads sysroot/usr/bin/malloctest sysroot/usr/bin/init sysroot/usr/bin/insmod sysroot/usr/bin/polltest sysroot/usr/bin/kill sysroot/usr/bin/time sysroot/usr/bin/strace sysroot/usr/bin/sleep sysroot/usr/bin/forks sysroot/usr/bin/multiread sysroot/usr/bin/threadsw sysroot/usr/bin/ab sysroot/usr/bin/write sysroot/usr/bin/segv sysroot/usr/bin/echo sysroot/usr/bin/sg sysroot/usr/bin/uthread sysroot/usr/bin/top sysroot/usr/bin/busy sysroot/usr/bin/rot13 sysroot/usr/bin/create sysroot/usr/bin/args sysroot/usr/bin/fcat sysroot/usr/bin/uname sysroot/usr/bin/float sysroot/usr/bin/fio sysroot/usr/bin/clone sysroot/usr/bin/bf2 sysroot/usr/bin/what

MP_ALL_INSTALL_TARGETS := sysroot/usr/lib/libc.a ngk.elf sysroot/usr/lib/crt0.o sysroot/usr/bin/sh sysroot/usr/bin/procmod.ko sysroot/usr/bin/testmod.ko sysroot/usr/bin/thread.ko sysroot/usr/bin/validate.ko sysroot/usr/bin/bf sysroot/usr/bin/bg sysroot/usr/bin/ls sysroot/usr/bin/syscall_test sysroot/usr/bin/bomb sysroot/usr/bin/column sysroot/usr/bin/trace sysroot/usr/bin/sigtest sysroot/usr/bin/head sysroot/usr/bin/crash sysroot/usr/bin/false sysroot/usr/bin/hog sysroot/usr/bin/threadss sysroot/usr/bin/test sysroot/usr/bin/pipe sysroot/usr/bin/cat sysroot/usr/bin/threads sysroot/usr/bin/malloctest sysroot/usr/bin/init sysroot/usr/bin/insmod sysroot/usr/bin/polltest sysroot/usr/bin/kill sysroot/usr/bin/time sysroot/usr/bin/strace sysroot/usr/bin/sleep sysroot/usr/bin/forks sysroot/usr/bin/multiread sysroot/usr/bin/threadsw sysroot/usr/bin/ab sysroot/usr/bin/write sysroot/usr/bin/segv sysroot/usr/bin/echo sysroot/usr/bin/sg sysroot/usr/bin/uthread sysroot/usr/bin/top sysroot/usr/bin/busy sysroot/usr/bin/rot13 sysroot/usr/bin/create sysroot/usr/bin/args sysroot/usr/bin/fcat sysroot/usr/bin/uname sysroot/usr/bin/float sysroot/usr/bin/fio sysroot/usr/bin/clone sysroot/usr/bin/bf2 sysroot/usr/bin/what

include $(shell find dep -name '*.d')
