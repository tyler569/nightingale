list(APPEND KERNEL_SOURCES
    arch/x86/acpi.c
    arch/x86/apic.c
    arch/x86/boot.S
    arch/x86/cpu.c
    arch/x86/halt.c
    arch/x86/interrupt.c
    arch/x86/isrs.S
    arch/x86/pic.c
    arch/x86/pit.c
    arch/x86/uart.c
    arch/x86/vmm.c
    debug.c
    dmgr.c
    elf.c
    exec.c
    fs/char_devices.c
    fs/directory.c
    fs/fs.c
    fs/membuf.c
    fs/pipe.c
    fs/procfs.c
    fs/socket.c
    irq.c
    main.c
    mman.c
    mod.c
    multiboot.c
    pci.c
    pmm.c
    rand.c
    ringbuf.c
    serial.c
    signal.c
    spalloc.c
    string.c
    sync.c
    sync_testbed.c
    syscall.c
    tarfs.c
    tests.c
    thread.c
    timer.c
    trace.c
    tty.c
    ubsan.c
    uname.c
    ../libc/ctype.c
    ../libc/errno.c
    ../libc/malloc.c
    ../libc/setjmp.S
    ../libc/signal.c
    ../libc/stdio.c
    ../libc/stdlib.c
    ../libc/string.c
    ../libc/x86_64/nightingale.c
    ../linker/elf-ng.c
    ../linker/modld.c
)

add_compile_definitions(__kernel__=1)

add_executable(nightingale_kernel ${KERNEL_SOURCES})

set(CMAKE_C_FLAGS ${KERNEL_CFLAGS})

set(CMAKE_C_LINK_FLAGS " \
    -T${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/link_hh.ld \
")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include")
