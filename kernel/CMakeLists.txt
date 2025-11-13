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
    ../libc/string.c
    ../libc/timeconv.c
    ../linker/elf-ng.c
    ../linker/modld.c
    chacha20.c
    commandline.c
    debug.c
    dmgr.c
    drv/e1000.c
    drv/rtl8139.c
    elf.c
    event_log.c
    exec.c
    font.c
    fs/char_dev.c
    fs/dentry.c
    fs/file_system.c
    fs/file.c
    fs/init.c
    fs/initfs.c
    fs/pipe.c
    fs/proc_files.c
    fs/proc.c
    fs/syscalls.c
    fs/tarfs.c
    fs/tmpfs.c
    fs/tty.c
    fs/vnode.c
    irq.c
    limine.c
    main.c
    mman.c
    mod.c
    mutex.c
    net/arp.c
    net/debug.c
    net/ip_egress.c
    net/ip_ingress.c
    net/net_ingress.c
    net/pk.c
    net/utils.c
    net/worker.c
    pci.c
    pmm.c
    print_test.c
    random.c
    ringbuf.c
    signal.c
    spalloc.c
    spin.c
    string.c
    sync_testbed.c
    syscall.c
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

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "X86_64")
    target_sources(nightingale_kernel PRIVATE
        ../arch/x86_64/acpi.c
        ../arch/x86_64/arch.c
        ../arch/x86_64/ata.c
        ../arch/x86_64/backtrace.c
        ../arch/x86_64/boot.S
        ../arch/x86_64/cpu.c
        ../arch/x86_64/gdt.c
        ../arch/x86_64/halt.c
        ../arch/x86_64/interrupt.c
        ../arch/x86_64/ioapic.c
        ../arch/x86_64/isrs.S
        ../arch/x86_64/lapic.c
        ../arch/x86_64/pic.c
        ../arch/x86_64/pit.c
        ../arch/x86_64/rtc.c
        ../arch/x86_64/serial.c
        ../arch/x86_64/uart.c
        ../arch/x86_64/vmm.c
        ../libc/x86_64/nightingale.c
        ../libc/x86_64/setjmp.S
    )

    set(NG_KERNEL_LINK_SCRIPT "${CMAKE_SOURCE_DIR}/arch/x86_64/link_hh.ld")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    target_sources(nightingale_kernel PRIVATE
    )

    set(NG_KERNEL_LINK_SCRIPT "${CMAKE_SOURCE_DIR}/arch/aarch64/link_hh.ld")
else ()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif ()

# Apply Nightingale kernel target properties
nightingale_kernel_target_properties(nightingale_kernel)
target_include_directories(nightingale_kernel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(nightingale_kernel PRIVATE autogen_headers limine_headers)

set_target_properties(nightingale_kernel PROPERTIES
    LINK_DEPENDS "${NG_KERNEL_LINK_SCRIPT}"
)

target_link_options(nightingale_kernel PRIVATE
    -T${NG_KERNEL_LINK_SCRIPT}
    -zmax-page-size=0x1000
    --gc-sections
)

install(TARGETS nightingale_kernel DESTINATION boot COMPONENT iso)
install(FILES ${CMAKE_SOURCE_DIR}/kernel/limine.cfg DESTINATION boot COMPONENT iso)
