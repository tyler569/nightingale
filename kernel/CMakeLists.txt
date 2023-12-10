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
    arch/x86/acpi.cpp
    arch/x86/arch.cpp
    arch/x86/boot.S
    arch/x86/cpu.c
    arch/x86/gdt.cpp
    arch/x86/halt.cpp
    arch/x86/interrupt.cpp
    arch/x86/ioapic.cpp
    arch/x86/isrs.S
    arch/x86/lapic.cpp
    arch/x86/pic.cpp
    arch/x86/pit.cpp
    arch/x86/rtc.cpp
    arch/x86/uart.cpp
    arch/x86/vmm.cpp
    block.cpp
    chacha20.cpp
    commandline.cpp
    cpp_test.cpp
    debug.cpp
    drv/pci_device.cpp
    drv/rtl8139.cpp
    elf.cpp
    exec.cpp
    ext2.cpp
    font.c
    fs/char_dev.cpp
    fs/dentry.cpp
    fs/ext2.cpp
    fs/file.cpp
    fs/file_system.cpp
    fs/init.cpp
    fs/initfs.cpp
    fs/inode.cpp
    fs/pipe.cpp
    fs/proc.cpp
    fs/syscalls.cpp
    fs/tmpfs.cpp
    fs/tty.cpp
    event_log.cpp
    irq.cpp
    limine.cpp
    main.cpp
    mman.cpp
    mod.cpp
    newmutex.cpp
    pci.cpp
    pmm.cpp
    proc_files.cpp
    random.cpp
    ringbuf.cpp
    serial.cpp
    signal.cpp
    spalloc.cpp
    spin.cpp
    string.cpp
    sync_testbed.c
    syscall.cpp
    tarfs.cpp
    tests.cpp
    thread.cpp
    time.cpp
    timer.cpp
    trace.cpp
    tty.cpp
    ubsan.cpp
    uname.cpp
    ../libc/ctype.c
    ../libc/errno.c
    ../libc/malloc.c
    ../libc/printf.c
    ../libc/qsort.c
    ../libc/setjmp.S
    ../libc/signal.c
    ../libc/stat.c
    ../libc/stdlib.c
    ../libc/string.c
    ../libc/timeconv.c
    ../libc/x86_64/nightingale.c
    ../libnx/print.cpp
    ../libnx/new.cpp
    ../linker/elf-ng.c
    ../linker/modld.c
    video.cpp
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
