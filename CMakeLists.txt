cmake_minimum_required(VERSION 3.10)

project(nightingale C ASM)

unset(CMAKE_SYSROOT)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/sysroot)
set(CMAKE_SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(CMAKE_ASM_COMPILER clang)
set(CMAKE_ASM_COMPILER_TARGET x86_64-unknown-none)
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET x86_64-unknown-none)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET x86_64-unknown-none)
set(CMAKE_LINKER ld.lld)
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

add_compile_options(
    -I../include
    -std=c11
    -Wall
    -Wextra
    -Werror
    -pedantic
    -g
    -O2
    -D__nightingale__=1
    -ffreestanding
    -nostdlib
    -Wno-unused-variable
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-sign-compare
    -Wno-address-of-packed-member
)

set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(include)

add_custom_target(initfs
    COMMAND tar cf TARGET_FILE ${CMAKE_SYSROOT}
)

add_subdirectory(kernel)
add_subdirectory(libc)
add_subdirectory(linker)
add_subdirectory(user)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
