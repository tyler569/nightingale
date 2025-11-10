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

    # Copy object file to .ko in lib directory
    add_custom_command(
        OUTPUT ${system_lib_dir}/${module_name}.ko
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_OBJECTS:module_${module_name}> ${system_lib_dir}/${module_name}.ko
        DEPENDS module_${module_name}
        VERBATIM
    )

    add_custom_target(${module_name}_ko ALL DEPENDS ${system_lib_dir}/${module_name}.ko)
    list(APPEND module_ko_files ${system_lib_dir}/${module_name}.ko)
endforeach ()

add_custom_target(kernel_modules DEPENDS ${module_ko_files})
