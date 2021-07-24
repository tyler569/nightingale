list(APPEND USERLAND_PROGRAMS
    ab args bf bf2 bg bomb busy cat cforks chmod clone column crash create dg
    echo false float forks head hog init insmod kill ls mmap modsys oom rm
    rot13 sc sleep st step strace test threads time top trace traceback uname
    uthread xd
)

set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

foreach(PROGRAM ${USERLAND_PROGRAMS})
    add_executable(${PROGRAM} ${PROGRAM}.c)
    target_link_libraries(${PROGRAM} crt0)
    target_link_libraries(${PROGRAM} c)
    install(TARGETS ${PROGRAM} RUNTIME DESTINATION bin)
    add_dependencies(initfs ${PROGRAM})
endforeach()

target_link_libraries(step elf)
