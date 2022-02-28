set(LIBC_SOURCES
    crt0.S
    crti.S
    crtn.S
    ctype.c
    entry.c
    errno.c
    fstdio.c
    fstdio_unlocked.c
    getopt.c
    locale.c
    malloc.c
    nightingale.c
    printf.c
    qsort.c
    setjmp.S
    signal.c
    stdlib.c
    string.c
    strtod.c
    syscall.c
    syscalls.c
    time.c
    timeconv.c
    todo.c
    unistd.c
    vector.c
    x86_64/nightingale.c
)

# add_library(crt0 STATIC crt0.S)
# install(TARGETS crt0 LIBRARY)

add_library(c STATIC ${LIBC_SOURCES})
# add_dependencies(c crt0)
install(TARGETS c LIBRARY)
