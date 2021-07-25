list(APPEND KERNEL_MODULES
    bss
    crash
    file
    procmod
    syscall
    testmod
    thread
)

set(CMAKE_C_FLAGS ${KERNEL_CFLAGS})

foreach(MODULE ${KERNEL_MODULES})
    add_library(module_${MODULE} OBJECT ${MODULE}.c)
    install(
        FILES $<TARGET_OBJECTS:module_${MODULE}>
        DESTINATION bin
        RENAME ${MODULE}.ko
    )
    add_dependencies(module_${MODULE} generate_headers)
endforeach()
