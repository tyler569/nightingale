list(APPEND kernel_modules_list
    bss
    crash
    file
    procmod
    syscall
    testmod
    thread
)

foreach (module_name ${kernel_modules_list})
    add_library(module_${module_name} OBJECT ${module_name}.c)

    # Apply kernel module properties
    nightingale_kernel_target_properties(module_${module_name})
    target_include_directories(module_${module_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/kernel/include
    )
    target_link_libraries(module_${module_name} PRIVATE autogen_headers)

    # Create .ko file in build directory
    set(ko_file ${CMAKE_CURRENT_BINARY_DIR}/${module_name}.ko)
    add_custom_command(
        OUTPUT ${ko_file}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_OBJECTS:module_${module_name}> ${ko_file}
        DEPENDS module_${module_name}
        VERBATIM
    )

    add_custom_target(${module_name}_ko ALL DEPENDS ${ko_file})
    list(APPEND module_ko_files ${ko_file})

    # Install .ko file to lib directory
    install(FILES ${ko_file} DESTINATION lib COMPONENT initrd)
endforeach ()

add_custom_target(kernel_modules DEPENDS ${module_ko_files})
