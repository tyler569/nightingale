file(GLOB_RECURSE LIB_SOURCES "*.rs")

set(BIN_FILE ${CMAKE_CURRENT_BINARY_DIR}/target/x86_64-unightingale/debug/basic)

add_custom_command(
    OUTPUT ${BIN_FILE}
    COMMAND cargo build -Zbuild-std --target-dir ${CMAKE_CURRENT_BINARY_DIR}/target
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${LIB_SOURCES}
)

add_custom_target(basic ALL DEPENDS ${BIN_FILE})

install(PROGRAMS ${BIN_FILE} DESTINATION bin)
