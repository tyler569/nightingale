list(APPEND KERNEL_MODULES
    bss
    crash
    file
    procmod
    syscall
    testmod
    thread
)

list(APPEND module_files)

foreach (MODULE ${KERNEL_MODULES})
    add_library(module_${MODULE} OBJECT ${MODULE}.c)
    set_target_properties(module_${MODULE} PROPERTIES
        OUTPUT_NAME ${MODULE}
    )
    add_dependencies(module_${MODULE} generate_headers)
    add_custom_command(OUTPUT ${system_lib_dir}/${MODULE}.ko
        COMMAND cp $<TARGET_OBJECTS:module_${MODULE}> ${system_lib_dir}/${MODULE}.ko
        DEPENDS module_${MODULE}
    )
    add_custom_target(${MODULE}.ko ALL DEPENDS ${system_lib_dir}/${MODULE}.ko)
    list(APPEND module_files ${system_lib_dir}/${MODULE}.ko)
endforeach ()

add_custom_target(kernel_modules DEPENDS ${module_files})
