add_compile_options(
    -fno-asynchronous-unwind-tables
    -fno-omit-frame-pointer
    -fsanitize=undefined
    ${NG_KERNEL_ARCH_CFLAGS}
)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "X86_64")
    set(ARCH_SOURCES
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
        ../libc/x86_64/setjmp.S
        ../libc/x86_64/nightingale.c
    )
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    set(ARCH_SOURCES
    )
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

add_compile_definitions(__kernel__=1 _NG_SOURCE=1)

add_subdirectory(modules)

add_executable(nightingale_kernel
    ${ARCH_SOURCES}
    ../libc/ctype.c
    ../libc/errno.c
    ../libc/malloc.c
    ../libc/printf.c
    ../libc/qsort.c
    ../libc/rbtree.c
    ../libc/signal.c
    ../libc/stat.c
    ../libc/stdlib.c
    ../libc/string.c
    ../libc/timeconv.c
    ../linker/elf-ng.c
    ../linker/modld.c
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

set_target_properties(nightingale_kernel PROPERTIES
    LINK_DEPENDS "${NG_KERNEL_LINK_SCRIPT}"
)

set(CMAKE_C_LINK_FLAGS " \
    -T${NG_KERNEL_LINK_SCRIPT} \
    -zmax-page-size=0x1000 \
")
