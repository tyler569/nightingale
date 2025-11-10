# Common functions and macros for Nightingale OS build system

# Apply standard Nightingale compile options to a target
function(nightingale_target_compile_options target_name)
    target_compile_options(${target_name} PRIVATE
        -Wall
        -Wextra
        -ffreestanding
        -nostdlib
        -Wno-unused-variable
        -Wno-unused-parameter
        -Wno-sign-compare
        -Wno-address-of-packed-member
        -Wno-deprecated-non-prototype
    )
endfunction()

# Apply standard Nightingale compile definitions to a target
function(nightingale_target_compile_definitions target_name)
    target_compile_definitions(${target_name} PRIVATE
        __nightingale__=1
        _NG_SOURCE=1
    )
endfunction()

# Apply both compile options and definitions in one call
function(nightingale_apply_target_properties target_name)
    nightingale_target_compile_options(${target_name})
    nightingale_target_compile_definitions(${target_name})
    target_include_directories(${target_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )
endfunction()

# Apply kernel-specific compile options
function(nightingale_kernel_target_properties target_name)
    nightingale_apply_target_properties(${target_name})

    target_compile_options(${target_name} PRIVATE
        -fno-asynchronous-unwind-tables
        -fno-omit-frame-pointer
    )

    if(nightingale_enable_ubsan)
        target_compile_options(${target_name} PRIVATE -fsanitize=undefined)
    endif()

    target_compile_definitions(${target_name} PRIVATE __kernel__=1)

    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "X86_64")
        target_compile_options(${target_name} PRIVATE
            -mno-red-zone
            -mno-mmx
            -mno-sse
            -mno-sse2
            -mcmodel=kernel
        )
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
        target_compile_options(${target_name} PRIVATE
            -mgeneral-regs-only
        )
    endif()
endfunction()

# Apply userland-specific compile options
function(nightingale_userland_target_properties target_name)
    nightingale_apply_target_properties(${target_name})

    target_compile_options(${target_name} PRIVATE
        -Wno-error=pedantic
        -fhosted
    )

    target_link_options(${target_name} PRIVATE -static)
endfunction()
