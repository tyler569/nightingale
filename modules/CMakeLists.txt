list(APPEND KERNEL_MODULES
    bss
    crash
    file
    procmod
    syscall
    testmod
    thread
)

foreach(MODULE ${KERNEL_MODULES})
    add_library(${MODULE} OBJECT ${MODULE}.c)
    install(TARGETS ${MODULE} RUNTIME DESTINATION bin)
endforeach()

target_link_libraries(step elf)
