NIGHTINGALE_VERSION=$(shell git describe --tags)

CFLAGS := \
	-std=c11 \
	-Wall \
	-Wextra \
	-Werror \
	-g \
	-O2 \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-function \
	-Wno-sign-compare \
	-Wno-address-of-packed-member \
	-pedantic \
	-ffreestanding \
	-mno-red-zone \
	-mno-80387 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-nostdlib \
	-fno-asynchronous-unwind-tables \
	-fno-omit-frame-pointer \
	-DNIGHTINGALE_VERSION="\"$(NIGHTINGALE_VERSION)\"" \
	-D__kernel__=1 \
	-D_NG=1 \
	-mcmodel=kernel \
	-fsanitize=undefined \

CSRC := \
	./ubsan.c \
	./spalloc.c \
	./dmgr.c \
	./mman.c \
	./elf.c \
	./rand.c \
	./thread.c \
	./multiboot.c \
	./sync_testbed.c \
	./tty.c \
	./pmm.c \
	./signal.c \
	./pci.c \
	./uname.c \
	./string.c \
	./tarfs.c \
	./syscall.c \
	./arch/x86/pic.c \
	./arch/x86/acpi.c \
	./arch/x86/interrupt.c \
	./arch/x86/halt.c \
	./arch/x86/uart.c \
	./arch/x86/vmm.c \
	./arch/x86/apic.c \
	./arch/x86/cpu.c \
	./arch/x86/pit.c \
	./mod.c \
	./sync.c \
	./timer.c \
	./fs/socket.c \
	./fs/procfs.c \
	./fs/directory.c \
	./fs/membuf.c \
	./fs/pipe.c \
	./fs/char_devices.c \
	./fs/fs.c \
	./exec.c \
	./tests.c \
	./main.c \
	./ringbuf.c \
	./serial.c \
	./debug.c \
	./trace.c \
	./irq.c \

COBJ := $(CSRC:.c=.c.o)

ASRC := \
	./arch/x86/boot.S \
	./arch/x86/isrs.S \

AOBJ := $(ASRC:.S=.S.o)

OBJECTS := $(COBJ) $(AOBJ)

ngk.elf: $(OBJECTS)
	ld.lld -Tarch/x86/link_hh.ld -o $@ $^

%.c.o: %.c
	clang -target x86_64-unknown-none -I../include $(CFLAGS) -o $@ -c $<

%.S.o: %.S
	clang -target x86_64-unknown-nong -I../include $(CFLAGS) -o $@ -c $<
