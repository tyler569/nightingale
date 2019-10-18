
CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

PROJECT(NIGHTINGALE_KERNEL C ASM_NASM)
SET(CAN_USE_ASSEMBLER TRUE)
ENABLE_LANGUAGE(ASM_NASM)

FILE(GLOB KERNEL_ARCH_X86_64_SRCS "arch/x86/*.c" "arch/x86/64/*.c" "arch/x86/64/*.asm")
FILE(GLOB KERNEL_ARCH_X86_32_SRCS "arch/x86/*.c" "arch/x86/32/*.c" "arch/x86/32/*.asm")

FILE(GLOB KERNEL_SRCS "drv/*.c" "ds/*.c" "fs/*.c" "kernel/*.c")

FILE(GLOB LIBC_SRCS "user/libc/*.c")
FILE(GLOB USER_SRCS "user/*.c")

EXECUTE_PROCESS(COMMAND git describe --tags
                OUTPUT_VARIABLE NG_VERSION
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)

SET(TARGET_LINK_SCRIPT "${CMAKE_SOURCE_DIR}/arch/x86/64/link_hh.ld")
SET(TARGET_CFLAGS "-mcmodel=kernel")


SET(CMAKE_C_FLAGS " \
    -I${CMAKE_SOURCE_DIR}/include -O0 -g -DDEBUG_KERNEL=1 \
    -Wall -Wextra -Werror -Wpedantic -pedantic \
    -std=c11 -nostdlib -ffreestanding -mgeneral-regs-only \
    -mno-red-zone -fno-asynchronous-unwind-tables \
    -fno-strict-aliasing -fno-omit-frame-pointer \
    -DNIGHTINGALE_VERSION=\"\\\"${NG_VERSION}\\\"\" \
    -D__nightingale__=1 -D__kernel__=1 \
    -Wno-unused-variable -Wno-unused-parameter \
    -Wno-sign-compare -Wno-unused-function \
    ${TARGET_CFLAGS}"
)

SET(CMAKE_ASM_FLAGS "")
SET(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

SET(CMAKE_EXE_LINKER_FLAGS " \
    -nostdlib -T${TARGET_LINK_SCRIPT} -zmax-page-size=0x1000 -g"
)

SET(CMAKE_C_COMPILER "x86_64-elf-gcc")
SET(CMAKE_LINKER "x86_elf_gcc")

ADD_EXECUTABLE(ngk ${KERNEL_SRCS} ${KERNEL_ARCH_X86_64_SRCS})

