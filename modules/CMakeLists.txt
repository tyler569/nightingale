cmake_minimum_required(VERSION 3.16)
project(nightingale-modules C)

# Add include directories for nightingale headers
include_directories(${CMAKE_INSTALL_PREFIX}/include)

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
endforeach ()

# Create targets list for kernel_modules
foreach (MODULE ${KERNEL_MODULES})
    list(APPEND module_targets module_${MODULE})
endforeach ()
add_custom_target(kernel_modules DEPENDS ${module_targets})

# Install modules
foreach (MODULE ${KERNEL_MODULES})
    install(FILES $<TARGET_OBJECTS:module_${MODULE}>
        DESTINATION lib/modules
        COMPONENT modules
        RENAME ${MODULE}.o)
endforeach ()
