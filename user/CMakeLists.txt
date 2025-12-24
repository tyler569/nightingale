list(APPEND userland_programs
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

foreach (program_name ${userland_programs})
    add_executable(${program_name} ${program_name}.c)

    # Apply userland target properties
    nightingale_userland_target_properties(${program_name})

    target_link_libraries(${program_name} PRIVATE c autogen_headers)
    target_link_options(${program_name} PRIVATE --gc-sections)

    install(TARGETS ${program_name} DESTINATION bin COMPONENT initrd)
endforeach ()

# Dynamic test program
add_executable(dhello dhello.c)
target_sources(dhello PRIVATE ${CMAKE_SOURCE_DIR}/libc/x86_64/crt0.S)
nightingale_userland_dynamic_target_properties(dhello)
target_link_libraries(dhello PRIVATE dyc autogen_headers)
target_link_options(dhello PRIVATE --gc-sections)
install(TARGETS dhello DESTINATION bin COMPONENT initrd)

# Export the list to parent scope so install_to_sysroot can depend on it
set(userland_programs ${userland_programs} dhello PARENT_SCOPE)

# Special cases with additional library dependencies
target_link_libraries(step PRIVATE elf)
# target_link_libraries(float PRIVATE m)

add_subdirectory(sh)
