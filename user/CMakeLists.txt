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
    mathtest
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
    udp_test
    uname
    uthread
)

set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

add_compile_options(-Wno-error=pedantic -fhosted)

foreach (PROGRAM ${USERLAND_PROGRAMS})
    add_executable(${PROGRAM} ${PROGRAM}.c)
    target_link_libraries(${PROGRAM} c)
endforeach ()

target_link_libraries(step elf)
target_link_libraries(mathtest m)
# target_link_libraries(float m)

add_subdirectory(sh)

# Add package management system
add_subdirectory(packages)

find_program(tar_command
    NAMES gtar tar
)

add_custom_command(OUTPUT ${initrd_file}
    DEPENDS ${USERLAND_PROGRAMS} sh hello_pkg lua_pkg luac_pkg libm_deploy
    COMMAND ${tar_command} cf ${initrd_file} -C ${iso_dir} .
    VERBATIM
)
add_custom_target(initrd DEPENDS ${initrd_file})
