file(GLOB_RECURSE LIB_SOURCES "*.rs")

set(LIB_FILE ${CMAKE_CURRENT_BINARY_DIR}/target/x86_64-nightingale/debug/libkernel_rs.a)

add_custom_command(
    OUTPUT ${LIB_FILE}
    COMMAND cargo build -Zbuild-std --target-dir ${CMAKE_CURRENT_BINARY_DIR}/target
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${LIB_SOURCES}
)

add_custom_target(kernel_rs_target ALL DEPENDS ${LIB_FILE})
add_library(kernel_rs STATIC IMPORTED GLOBAL)
add_dependencies(kernel_rs kernel_rs_target)
set_target_properties(kernel_rs PROPERTIES IMPORTED_LOCATION ${LIB_FILE})

