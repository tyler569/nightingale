if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "X86_64")
    set(ARCH_SOURCES
        x86_64/crt0.S
        x86_64/crti.S
        x86_64/crtn.S
        x86_64/setjmp.S
        x86_64/nightingale.c
    )
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    set(ARCH_SOURCES
    )
else ()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif ()

set(LIBC_SOURCES
    ${ARCH_SOURCES}
    ctype.c
    entry.c
    errno.c
    fs2.c
    fstdio.c
    fstdio_unlocked.c
    getopt.c
    hexdump.c
    locale.c
    malloc.c
    nightingale.c
    printf.c
    qsort.c
    signal.c
    stat.c
    stdlib.c
    stream.c
    stream_ring.c
    string.c
    syscalls.c
    time.c
    timeconv.c
    todo.c
    unistd.c
)

add_compile_definitions(_NG_SOURCE=1)

add_library(c STATIC ${LIBC_SOURCES})
# add_library(dyc SHARED ${LIBC_SOURCES})
# set_target_properties(dyc PROPERTIES OUTPUT_NAME c)
