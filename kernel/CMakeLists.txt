add_compile_options(
    -fno-asynchronous-unwind-tables
    -fno-omit-frame-pointer
    -fsanitize=undefined
)

add_compile_definitions(__kernel__=1 _NG_SOURCE=1)

add_subdirectory(modules)

add_executable(nightingale_kernel
    ../libc/ctype.c
    ../libc/errno.c
    ../libc/hexdump.c
    ../libc/malloc.c
    ../libc/print.c
    ../libc/qsort.c
    ../libc/rbtree.c
    ../libc/signal.c
    ../libc/stat.c
    ../libc/stdlib.c
    ../libc/stream.c
    ../libc/stream_ring.c
    ../libc/string.c
    ../libc/timeconv.c
    ../linker/elf-ng.c
    ../linker/modld.c
    drv/e1000.c
    drv/irq.c
    drv/pci.c
    drv/rtl8139.c
    fs/char_dev.c
    fs/dentry.c
    fs/file.c
    fs/file_system.c
    fs/init.c
    fs/initfs.c
    fs/pipe.c
    fs/proc.c
    fs/proc_files.c
    fs/syscalls.c
    fs/tarfs.c
    fs/tmpfs.c
    fs/tty.c
    fs/vnode.c
    lib/chacha20.c
    lib/commandline.c
    lib/debug.c
    lib/event_log.c
    lib/font.c
    lib/limine.c
    lib/mod.c
    lib/print_test.c
    lib/random.c
    lib/string.c
    lib/trace.c
    lib/ubsan.c
    main.c
    lib/dmgr.c
    mem/mman.c
    mem/pmm.c
    mem/ringbuf.c
    mem/spalloc.c
    mt/elf.c
    mt/exec.c
    mt/mutex.c
    mt/signal.c
    mt/spin.c
    mt/sync_testbed.c
    mt/thread_fini.c
    mt/thread_fork.c
    mt/thread_init.c
    mt/thread_sched.c
    mt/thread_sleep.c
    mt/thread_utils.c
    mt/thread_wait.c
    net/debug.c
    net/pk.c
    net/utils.c
    syscall.c
    tests.c
    time.c
    timer.c
    tty.c
    uname.c
    video.c
    net/ip_ingress.c
    net/arp.c
    net/net_ingress.c
)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "X86_64")
    target_sources(nightingale_kernel PRIVATE
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

    target_compile_options(nightingale_kernel PRIVATE
        -mno-red-zone
        -mno-mmx
        -mno-sse
        -mno-sse2
        -mcmodel=kernel
    )

    set(NG_KERNEL_LINK_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/link_hh.ld")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    target_sources(nightingale_kernel PRIVATE
    )

    target_compile_options(nightingale_kernel PRIVATE
        -mgeneral-regs-only
    )

    set(NG_KERNEL_LINK_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/arch/aarch64/link_hh.ld")
else ()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif ()

set_target_properties(nightingale_kernel PROPERTIES
    LINK_DEPENDS "${NG_KERNEL_LINK_SCRIPT}"
)

set(CMAKE_C_LINK_FLAGS " \
    -T${NG_KERNEL_LINK_SCRIPT} \
    -zmax-page-size=0x1000 \
")
