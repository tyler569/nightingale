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
endforeach ()

# Special cases with additional library dependencies
target_link_libraries(step PRIVATE elf)
# target_link_libraries(float PRIVATE m)

add_subdirectory(sh)

find_program(tar_command
    NAMES gtar tar
)

add_custom_command(OUTPUT ${initrd_file}
    DEPENDS ${userland_programs} sh
    COMMAND ${tar_command} cf ${initrd_file} -C ${iso_dir} .
    VERBATIM
)
add_custom_target(initrd DEPENDS ${initrd_file})
