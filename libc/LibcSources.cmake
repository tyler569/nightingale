# Shared libc source list used by both kernel and userland

# Architecture-specific sources
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "X86_64")
    set(libc_arch_sources
        x86_64/crt0.S
        x86_64/crti.S
        x86_64/crtn.S
        x86_64/setjmp.S
        x86_64/nightingale.c
    )
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    set(libc_arch_sources
        # TODO: Add aarch64 sources
    )
else ()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif ()

# Common libc sources (architecture-independent)
set(libc_common_sources
    ctype.c
    errno.c
    hexdump.c
    malloc.c
    qsort.c
    signal.c
    stat.c
    stdlib.c
    stream.c
    stream_ring.c
    string.c
    timeconv.c
)

# Sources only needed for userland (not embedded in kernel)
set(libc_userland_only_sources
    entry.c
    fs2.c
    fstdio.c
    fstdio_unlocked.c
    getopt.c
    locale.c
    nightingale.c
    printf.c
    syscalls.c
    time.c
    todo.c
    unistd.c
)

# Sources embedded in kernel (subset of common)
set(libc_kernel_embedded_sources
    ctype.c
    errno.c
    hexdump.c
    malloc.c
    print.c
    qsort.c
    rbtree.c
    signal.c
    stat.c
    stdlib.c
    stream.c
    stream_ring.c
    string.c
    timeconv.c
)
