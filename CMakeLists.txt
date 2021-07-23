cmake_minimum_required(VERSION 3.10)

project(nightingale)

add_compile_options(
    -target x86_64-unknown-none
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

include_directories(include)

add_subdirectory(kernel)
