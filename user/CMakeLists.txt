list(APPEND USERLAND_PROGRAMS
    ab args bf bf2 bg bomb busy cat cforks chmod clear clone column
    crash create echo false float forks head hexdump hog init insmod
    kill ls mmap modsys oom rm rot13 sleep step strace test threads time
    top trace traceback uname uthread
)

set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

add_compile_options(-Wno-error=pedantic -fhosted)

foreach(PROGRAM ${USERLAND_PROGRAMS})
    add_executable(${PROGRAM} ${PROGRAM}.c)
    target_link_libraries(${PROGRAM} c)
    install(TARGETS ${PROGRAM} RUNTIME DESTINATION bin)
endforeach()

target_link_libraries(step elf)
target_link_libraries(float m)

add_subdirectory(sh)

install(FILES text_file DESTINATION bin)
install(
    FILES fib.lua
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin
)
