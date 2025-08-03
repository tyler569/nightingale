cmake_minimum_required(VERSION 3.10)
project(nightingale_libc C ASM)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "X86_64")
    set(ARCH_SOURCES
        x86_64/setjmp.S
        x86_64/nightingale.c
    )
    set(CRT_SOURCES
        x86_64/crt0.S
        x86_64/crti.S
        x86_64/crtn.S
    )
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    set(ARCH_SOURCES
    )
    set(CRT_SOURCES
    )
else ()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif ()

set(LIBC_SOURCES
    ${ARCH_SOURCES}
    ../shared/libc/ctype.c
    entry.c
    ../shared/libc/errno.c
    fs2.c
    fstdio.c
    fstdio_unlocked.c
    getopt.c
    ../shared/libc/hexdump.c
    locale.c
    ../shared/libc/malloc.c
    nightingale.c
    printf.c
    ../shared/libc/qsort.c
    ../shared/libc/signal.c
    ../shared/libc/stat.c
    ../shared/libc/stdlib.c
    ../shared/libc/stream.c
    ../shared/libc/stream_ring.c
    ../shared/libc/string.c
    syscalls.c
    time.c
    ../shared/libc/timeconv.c
    todo.c
    unistd.c
)

add_compile_definitions(_NG_SOURCE=1)

add_library(c STATIC ${LIBC_SOURCES})
# add_library(dyc SHARED ${LIBC_SOURCES})
# set_target_properties(dyc PROPERTIES OUTPUT_NAME c)

# Build and install individual CRT objects for x86_64
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "X86_64")
    # Build CRT objects as custom targets
    add_custom_command(
        OUTPUT crt0.o
        COMMAND ${CMAKE_ASM_COMPILER} -c -target x86_64-unknown-none 
                -o ${CMAKE_CURRENT_BINARY_DIR}/crt0.o 
                ${CMAKE_CURRENT_SOURCE_DIR}/x86_64/crt0.S
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/x86_64/crt0.S
        COMMENT "Building crt0.o"
    )
    add_custom_command(
        OUTPUT crti.o
        COMMAND ${CMAKE_ASM_COMPILER} -c -target x86_64-unknown-none 
                -o ${CMAKE_CURRENT_BINARY_DIR}/crti.o 
                ${CMAKE_CURRENT_SOURCE_DIR}/x86_64/crti.S
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/x86_64/crti.S
        COMMENT "Building crti.o"
    )
    add_custom_command(
        OUTPUT crtn.o
        COMMAND ${CMAKE_ASM_COMPILER} -c -target x86_64-unknown-none 
                -o ${CMAKE_CURRENT_BINARY_DIR}/crtn.o 
                ${CMAKE_CURRENT_SOURCE_DIR}/x86_64/crtn.S
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/x86_64/crtn.S
        COMMENT "Building crtn.o"
    )
    
    add_custom_target(crt_objects ALL 
        DEPENDS crt0.o crti.o crtn.o
        COMMENT "Building CRT objects"
    )
    
    # Install CRT objects
    install(FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/crt0.o
        ${CMAKE_CURRENT_BINARY_DIR}/crti.o
        ${CMAKE_CURRENT_BINARY_DIR}/crtn.o
        DESTINATION usr/lib
        COMPONENT libc
    )
endif()

# Install targets
install(TARGETS c
    ARCHIVE DESTINATION usr/lib
    COMPONENT libc)

# Install headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION usr/include
    COMPONENT libc-dev
    FILES_MATCHING PATTERN "*.h")
