add_compile_options(
    -mno-red-zone
    -mno-80387
    -mno-mmx
    -mno-sse
    -mno-sse2
    -mcmodel=kernel
    -fno-asynchronous-unwind-tables
    -fno-omit-frame-pointer
    # -fsanitize=undefined
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
    arch/x86/uart.c
    arch/x86/vmm.c
    chacha20.c
    commandline.c
    debug.c
    dmgr.c
    elf.c
    event_log.c
    exec.c
    ext2.c
    font.c
    fs/char_dev.c
    fs/dentry.c
    fs/ext2.c
    fs/file.c
    fs/file_system.c
    fs/init.c
    fs/initfs.c
    fs/inode.c
    fs/pipe.c
    fs/proc.c
    fs/syscalls.c
    fs/tmpfs.c
    fs/tty.c
    irq.c
    limine.c
    main.c
    mman.c
    mod.c
    multiboot.c
    newmutex.c
    pci.c
    pmm.c
    print.c
    proc_files.c
    random.c
    ringbuf.c
    serial.c
    signal.c
    spalloc.c
    spin.c
    string.c
    sync_testbed.c
    syscall.c
    tarfs.c
    tests.c
    thread.c
    time.c
    timer.c
    trace.c
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
