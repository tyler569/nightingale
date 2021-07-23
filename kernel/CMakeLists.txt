cmake_minimum_required(VERSION 3.10)

project(nightingale_kernel C ASM)

set(CMAKE_ASM_COMPILER clang)
set(CMAKE_C_COMPILER clang)
set(CMAKE_LINKER ld.lld)
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

add_executable(nightingale_kernel
    ./arch/x86/acpi.c
    ./arch/x86/apic.c
    ./arch/x86/boot.S
    ./arch/x86/cpu.c
    ./arch/x86/halt.c
    ./arch/x86/interrupt.c
    ./arch/x86/isrs.S
    ./arch/x86/pic.c
    ./arch/x86/pit.c
    ./arch/x86/uart.c
    ./arch/x86/vmm.c
    ./debug.c
    ./dmgr.c
    ./elf.c
    ./exec.c
    ./fs/char_devices.c
    ./fs/directory.c
    ./fs/fs.c
    ./fs/membuf.c
    ./fs/pipe.c
    ./fs/procfs.c
    ./fs/socket.c
    ./irq.c
    ./main.c
    ./mman.c
    ./mod.c
    ./multiboot.c
    ./pci.c
    ./pmm.c
    ./rand.c
    ./ringbuf.c
    ./serial.c
    ./signal.c
    ./spalloc.c
    ./string.c
    ./sync.c
    ./sync_testbed.c
    ./syscall.c
    ./tarfs.c
    ./tests.c
    ./thread.c
    ./timer.c
    ./trace.c
    ./tty.c
    ./ubsan.c
    ./uname.c
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

set(CMAKE_C_FLAGS " \
    -mno-red-zone \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -DNIGHTINGALE_VERSION=\"\\\"test\\\"\" \
    -D__kernel__=1 \
    -mcmodel=kernel \
    -fno-asynchronous-unwind-tables \
    -fno-omit-frame-pointer \
    -fsanitize=undefined \
")

# add_compile_options(
#     -mno-red-zone
#     -mno-80387
#     -mno-mmx
#     -mno-sse
#     -mno-sse2
#     -DNIGHTINGALE_VERSION=\"\\\"test\\\"\"
#     -D__kernel__=1
#     -mcmodel=kernel
#     -fno-asynchronous-unwind-tables
#     -fno-omit-frame-pointer
#     -fsanitize=undefined
# )

set(CMAKE_C_LINK_FLAGS " \
    --no-check-sections \
    -T${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/link_hh.ld \
")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include")
