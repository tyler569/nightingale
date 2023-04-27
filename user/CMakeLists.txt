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
    float
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
    install(TARGETS ${PROGRAM} RUNTIME DESTINATION bin)
endforeach ()

# Need headers to support C89 first
# set_property(TARGET cdecl PROPERTY C_STANDARD 90)

target_link_libraries(step elf)
target_link_libraries(float m)

add_subdirectory(sh)

install(FILES text_file DESTINATION bin)
install(
    FILES fib.lua
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin
)
