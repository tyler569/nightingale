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
    oom
    pwd
    report
    rm
    rot13
    sleep
    stat
    step
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
    uname
    uthread
)

set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

add_compile_options(-Wno-error=pedantic -fhosted)
add_compile_definitions(_NG_SOURCE=1)

foreach (PROGRAM ${USERLAND_PROGRAMS})
    add_executable(${PROGRAM} ${PROGRAM}.c)
    target_link_libraries(${PROGRAM} c)
endforeach ()

target_link_libraries(step elf)
# target_link_libraries(float m)

add_subdirectory(sh)

install(FILES text_file DESTINATION bin)

add_custom_command(OUTPUT initrd.tar
    DEPENDS ${USERLAND_PROGRAMS}
    COMMAND tar cf initrd.tar -C ${CMAKE_SYSROOT} .
)
add_custom_target(initrd DEPENDS initrd.tar)
