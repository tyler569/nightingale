# Hello World package - simple local build example

set(HELLO_PACKAGE_DIR "${CMAKE_BINARY_DIR}/packages/hello_world")
set(HELLO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Copy source files to build directory
file(MAKE_DIRECTORY "${HELLO_PACKAGE_DIR}")
file(COPY "${HELLO_SRC_DIR}/" DESTINATION "${HELLO_PACKAGE_DIR}/")

# Apply patches
apply_patches(hello_world)

# Build the hello world executable
add_executable(hello_pkg "${HELLO_PACKAGE_DIR}/hello.c")
target_link_libraries(hello_pkg c)

# Install executable to bin directory
install(TARGETS hello_pkg DESTINATION bin)

# Create the main build target that everything depends on
add_custom_target(hello_world_build DEPENDS hello_pkg)

# Create sysroot installation target (runs at build time)
add_custom_target(hello_world_install_to_sysroot
    COMMAND ${CMAKE_COMMAND} -E echo "hello_world has no sysroot artifacts to install"
    DEPENDS hello_world_build
    COMMENT "hello_world sysroot installation (no-op)"
)