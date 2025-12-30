# Find Rust toolchain
find_program(CARGO_COMMAND cargo REQUIRED)
find_program(RUSTC_COMMAND rustc REQUIRED)

# Rust target specification - use built-in x86_64-unknown-none
set(RUST_TARGET "x86_64-unknown-none")
set(RUST_TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/target")

# Rust flags to match kernel compilation requirements
set(RUST_FLAGS "-C code-model=kernel -C relocation-model=static")

# Build profile selection based on CMAKE_BUILD_TYPE
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(RUST_BUILD_PROFILE "release")
    set(RUST_PROFILE_FLAG "--release")
else()
    set(RUST_BUILD_PROFILE "debug")
    set(RUST_PROFILE_FLAG "")
endif()

# List of Rust modules to build
list(APPEND rust_modules_list
    hello_rust_mod
)

foreach(rust_module ${rust_modules_list})
    set(module_dir "${CMAKE_CURRENT_SOURCE_DIR}/${rust_module}")
    set(module_lib "${RUST_TARGET_DIR}/${RUST_TARGET}/${RUST_BUILD_PROFILE}/lib${rust_module}.a")
    set(module_ko "${CMAKE_CURRENT_BINARY_DIR}/${rust_module}.ko")
    set(temp_dir "${CMAKE_CURRENT_BINARY_DIR}/temp_${rust_module}")

    # Build Rust static library with cargo
    add_custom_command(
        OUTPUT ${module_lib}
        COMMAND ${CMAKE_COMMAND} -E env RUSTFLAGS=${RUST_FLAGS}
            ${CARGO_COMMAND} build
            --target ${RUST_TARGET}
            --target-dir ${RUST_TARGET_DIR}
            ${RUST_PROFILE_FLAG}
            --manifest-path ${module_dir}/Cargo.toml
        DEPENDS
            ${module_dir}/Cargo.toml
            ${module_dir}/src/lib.rs
        WORKING_DIRECTORY ${module_dir}
        COMMENT "Building Rust module: ${rust_module}"
        VERBATIM
    )

    # Convert static library to relocatable object (.ko)
    # Extract object files from the archive and combine them
    add_custom_command(
        OUTPUT ${module_ko}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${temp_dir}
        COMMAND cd ${temp_dir} && ar x ${module_lib}
        COMMAND sh -c "ld.lld -r -o ${module_ko} ${temp_dir}/*.o"
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${temp_dir}
        DEPENDS ${module_lib}
        COMMENT "Creating kernel module: ${rust_module}.ko"
    )

    add_custom_target(${rust_module}_rust_ko ALL DEPENDS ${module_ko})
    list(APPEND rust_module_ko_files ${module_ko})

    # Install .ko file to lib directory (same as C modules)
    install(FILES ${module_ko} DESTINATION lib COMPONENT initrd)
endforeach()

add_custom_target(rust_kernel_modules DEPENDS ${rust_module_ko_files})
