add_compile_options(
    -mno-red-zone
    -mno-80387
    -mno-mmx
    -mno-sse
    -mno-sse2
    -mcmodel=kernel
    -fno-asynchronous-unwind-tables
    -fno-omit-frame-pointer
    -fsanitize=undefined
)

add_compile_definitions(__kernel__=1 _NG_SOURCE=1)

add_subdirectory(modules)

add_executable(nightingale_kernel
    ../libc/ctype.c
    ../libc/errno.c
    ../libc/malloc.c
    ../libc/printf.c
    ../libc/qsort.c
    ../libc/rbtree.c
    ../libc/setjmp.S
    ../libc/signal.c
    ../libc/stat.c
    ../libc/stdlib.c
    ../libc/string.c
    ../libc/timeconv.c
    ../libc/x86_64/nightingale.c
    ../linker/elf-ng.c
    ../linker/modld.c
    arch/x86/acpi.c
    arch/x86/arch.c
    arch/x86/ata.c
    arch/x86/backtrace.c
    arch/x86/boot.S
    arch/x86/cpu.c
    arch/x86/gdt.c
    arch/x86/halt.c
    arch/x86/interrupt.c
    arch/x86/ioapic.c
    arch/x86/isrs.S
    arch/x86/lapic.c
    arch/x86/pic.c
    arch/x86/pit.c
    arch/x86/rtc.c
    arch/x86/serial.c
    arch/x86/uart.c
    arch/x86/vmm.c
    chacha20.c
    commandline.c
    debug.c
    drv/irq.c
    drv/pci.c
    drv/rtl8139.c
    event_log.c
    font.c
    fs/char_dev.c
    fs/dentry.c
    fs/ext2.c
    fs/ext2_util.c
    fs/file.c
    fs/file_system.c
    fs/init.c
    fs/initfs.c
    fs/inode.c
    fs/pipe.c
    fs/proc.c
    fs/proc_files.c
    fs/syscalls.c
    fs/tarfs.c
    fs/tmpfs.c
    fs/tty.c
    limine.c
    main.c
    mem/dmgr.c
    mem/mman.c
    mem/pmm.c
    mem/ringbuf.c
    mem/spalloc.c
    mod.c
    mt/elf.c
    mt/exec.c
    mt/signal.c
    mt/spin.c
    mt/sync_testbed.c
    mt/thread.c
    mt/trace.c
    mt/mutex.c
    print.c
    random.c
    string.c
    syscall.c
    tests.c
    time.c
    timer.c
    tty.c
    ubsan.c
    uname.c
    video.c
)

set(CMAKE_C_FLAGS ${KERNEL_CFLAGS})
set(CMAKE_CXX_FLAGS ${KERNEL_CFLAGS})

set(CMAKE_C_LINK_FLAGS " \
    -T${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/link_hh.ld \
    -zmax-page-size=0x1000 \
")

set(CMAKE_CXX_LINK_FLAGS " \
    -T${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/link_hh.ld \
    -zmax-page-size=0x1000 \
")
