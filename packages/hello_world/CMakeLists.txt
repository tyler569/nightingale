# Hello World package - simple local build example

set(HELLO_PACKAGE_DIR "${CMAKE_BINARY_DIR}/packages/hello_world")
set(HELLO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Copy source files to build directory
file(MAKE_DIRECTORY "${HELLO_PACKAGE_DIR}")
file(COPY "${HELLO_SRC_DIR}/" DESTINATION "${HELLO_PACKAGE_DIR}/")

# Build the hello world executable
add_executable(hello_pkg "${HELLO_PACKAGE_DIR}/hello.c")
target_link_libraries(hello_pkg c)

# Create the main build target
add_custom_target(hello_world_build DEPENDS hello_pkg)