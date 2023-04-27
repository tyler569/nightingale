set(LIBC_SOURCES
    crt0.S
    crti.S
    crtn.S
    ctype.c
    entry.c
    errno.c
    fs2.c
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
    stat.c
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

add_compile_definitions(_NG_SOURCE=1)

add_library(c STATIC ${LIBC_SOURCES})
# add_library(dyc SHARED ${LIBC_SOURCES})
# set_target_properties(dyc PROPERTIES OUTPUT_NAME c)
install(TARGETS c LIBRARY)
# install(TARGETS dyc LIBRARY)
