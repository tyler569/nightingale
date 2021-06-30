#!/usr/bin/env ruby

require_relative './magpie'

DYNAMIC = false

COMMON_FLAGS = [
  "-Wall",
  "-Wextra",
  "-Werror",
  "-g",
  "-O2",
  "-Wno-unused-variable",
  "-Wno-unused-parameter",
  "-Wno-unused-function",
  "-Wno-sign-compare",
  "-Wno-address-of-packed-member",
]

COMMON_CFLAGS = [
  "-std=c11",
  *COMMON_FLAGS,
  "$(CFLAGS)",
]

COMMON_CXXFLAGS = [
  "-std=c++2a",
  "-fno-exceptions",
  *COMMON_FLAGS,
  "$(CXXFLAGS)",
]

USER_CFLAGS = [
  *COMMON_CFLAGS,
  "-Wno-builtin-declaration-mismatch",
  "$(USER_CFLAGS)",
]

USER_CFLAGS << "-static" unless DYNAMIC

USER_CXXFLAGS = [
  *COMMON_CXXFLAGS,
  "-Wno-builtin-declaration-mismatch",
  "$(USER_CXXFLAGS)",
]

USER_CXXFLAGS << "-static" unless DYNAMIC

USER_LDFLAGS = [
  "-g",
]

USER_LDFLAGS << "-static" unless DYNAMIC

KERNEL_CFLAGS = [
  *COMMON_CFLAGS,
  "-pedantic",
  "-ffreestanding",
  "-mno-red-zone",
  "-mno-80387",
  "-mno-mmx",
  "-mno-sse",
  "-mno-sse2",
  "-nostdlib",
  "-fno-asynchronous-unwind-tables",
  "-fno-omit-frame-pointer",
  "-DNIGHTINGALE_VERSION=\\\"$(NIGHTINGALE_VERSION)\\\"",
  "-D__kernel__=1",
  "-D_NG=1",
  "-mcmodel=kernel",
  "-fsanitize=undefined",
  "$(KERNEL_CFLAGS)",
]

KERNEL_LDFLAGS = [
  "-nostdlib",
  "-Tx86/link_hh.ld",
  "-zmax-page-size=0x1000",
  "-g",
]

KERNEL_ASFLAGS = [
  "-felf64",
]

kernel_buildmode = {
  cc: gcc_compiler("x86_64-nightingale-gcc", default_flags: KERNEL_CFLAGS),
  as: nasm_assembler,
  ld: gcc_linker("x86_64-nightingale-gcc")
}

libk_buildmode = kernel_buildmode.merge(ld: ar_linker("x86_64-nightingale-ar"))

module_buildmode = kernel_buildmode.merge(ld: ld_r_linker("x86_64-nightingale-ld"))

user_buildmode = {
  cc: gcc_compiler("x86_64-nightingale-gcc", default_flags: USER_CFLAGS),
  cxx: gcc_compiler("x86_64-nightingale-g++", default_flags: USER_CXXFLAGS),
  as: gas_assembler("x86_64-nightingale-gcc"),
  ld: gcc_linker("x86_64-nightingale-gcc", default_flags: USER_LDFLAGS),
}

libc_buildmode = user_buildmode.merge(ld: ar_linker("x86_64-nightingale-ar"))

crt_buildmode = user_buildmode.merge(ld: ld_r_linker("x86_64-nightingale-ld"))

LIBK_SOURCES = [
  "libc/ctype.c",
  "libc/errno.c",
  "libc/malloc.c",
  "libc/setjmp.S",
  "libc/signal.c",
  "libc/stdlib.c",
  "libc/stdio.c",
  "libc/string.c",
  "libc/x86_64/nightingale.c",
]

KERNEL_SOURCES = [
  "kernel/**/*.c",
  "linker/elf-ng.c",
  "linker/modld.c",
  "fs/**/*.c",
  "x86/**/*.asm",
  "x86/**/*.c",
]

build = Build.new "nightingale"
build.define_target "libc.a", libc_buildmode, ["libc/**/*.[cS]"]
build.define_target "libm.a", libc_buildmode, ["libm/**/*.[cS]"]
build.define_target "libelf.a", libc_buildmode, ["linker/elf-ng.c"]
build.define_target "libk.a", libk_buildmode, LIBK_SOURCES
build.define_target "ngk.elf", kernel_buildmode, KERNEL_SOURCES, link: ["k"], install: "."
build.define_target "crt0.o", crt_buildmode, ["libc/crt0.S"], install: "sysroot/usr/lib"
build.define_target "sh", user_buildmode, ["sh/**/*.c"], install: "sysroot/bin"

puts build.render
p build

