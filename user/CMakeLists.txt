cmake_minimum_required(VERSION 3.10)
project(nightingale_coreutils C)

list(APPEND USERLAND_PROGRAMS
    ab
    args
    bf
    bf2
    bg
    bgfifo
    bomb
    busy
    cat
    cdecl
    cforks
    chmod
    clear
    clone
    column
    cpuid
    crash
    create
    date
    echo
    false
    forks
    fs2
    head
    hexdump
    hog
    init
    insmod
    io
    kill
    ls
    mmap
    modsys
    netfilter
    oom
    pwd
    report
    rm
    rot13
    sleep
    stat
    strace
    submit
    test
    thread_local
    threads
    time
    top
    trace
    traceback
    tree
    udp_test
    uname
    uthread
)

add_compile_options(-Wno-error=pedantic -fhosted)

foreach (PROGRAM ${USERLAND_PROGRAMS})
    add_executable(${PROGRAM} ${PROGRAM}.c)
    target_link_libraries(${PROGRAM} c)
endforeach ()

# target_link_libraries(step elf)  # Will add back when we build libelf
# target_link_libraries(mathtest m)  # Will add back when we build libm

# sh and packages are handled as separate packages now

# Install all executables
foreach (PROGRAM ${USERLAND_PROGRAMS})
    install(TARGETS ${PROGRAM}
        RUNTIME DESTINATION usr/bin
        COMPONENT coreutils)
endforeach ()
