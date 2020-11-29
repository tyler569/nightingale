#!/usr/bin/env ruby

require_relative 'magpie_build'

VERSION = `git describe --tags`.strip

DYNAMIC = false

COMMON_CFLAGS = [
  "-std=c11",
  "-Wall",
  "-Wextra",
  "-Werror",
  "-pedantic",
  "-g",
  "-Og",
  "-Wno-unused-variable",
  "-Wno-unused-parameter",
  "-Wno-unused-function",
  "-Wno-sign-compare",
  "-Wno-address-of-packed-member",
]

USER_CFLAGS = [
  *COMMON_CFLAGS,
  "-Wno-builtin-declaration-mismatch",
  # "-static",
]

USER_CFLAGS << "-static" unless DYNAMIC

USER_LDFLAGS = [
  # "-static",
  "-g",
]

USER_LDFLAGS << "-static" unless DYNAMIC

KERNEL_CFLAGS = [
  *COMMON_CFLAGS,
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
  "-D__kernel__=1 -D_NG=1",
  "-mcmodel=kernel",
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

build = MagpieBuild.define do
  build_dir "build-x86_64"

  mode :user do
    cflags USER_CFLAGS
    ldflags USER_LDFLAGS
    cc "x86_64-nightingale-gcc"
    ld "x86_64-nightingale-gcc"
  end

  mode :libc do
    cflags USER_CFLAGS
    ldflags USER_LDFLAGS
    cc "x86_64-nightingale-gcc"
    ld "ar"
  end

  mode :crt do
    as "x86_64-nightingale-gcc"
    ld nil
  end

  mode :so do
    # TODO factor this into COMMON -- it has no Wextra or pedanic
    # because that hits printf("%p\n", (non-void *)) -- I'm not sure
    # how I feel about that (and it's just "Werror=format")
    cflags [
      "-std=c11",
      "-Wall",
      "-Werror",
      "-Wno-unused-variable",
      "-Wno-unused-parameter",
      "-Wno-unused-function",
      "-Wno-sign-compare",
      "-Wno-address-of-packed-member",
      "-g",
      "-Og",
      "-nostdlib",
      "-fpic",
      "-shared",
    ]
    ldflags [
      "-nostdlib",
      "-fpic",
      "-shared",
      "-g",
    ]
    cc "x86_64-nightingale-gcc"
    ld "x86_64-nightingale-gcc"
  end

  mode :kernel do
    cflags KERNEL_CFLAGS
    ldflags KERNEL_LDFLAGS + ["-L#{build_dir}"]
    asflags KERNEL_ASFLAGS
    as "nasm"
    cc "x86_64-nightingale-gcc"
    ld "x86_64-nightingale-gcc"
  end

  mode :libk do
    cflags KERNEL_CFLAGS
    asflags KERNEL_CFLAGS
    as "x86_64-nightingale-gcc"
    cc "x86_64-nightingale-gcc"
    ld "ar"
  end

  mode :module do
    cflags KERNEL_CFLAGS
    cc "x86_64-nightingale-gcc"
    ld nil
  end

  target "libc.a" do
    language "C", "asm"
    mode :libc
    sources "libc/**/*.c", "libc/**/*.S"
    install "sysroot/usr/lib"
  end

  target "libc.so" do
    language "C", "asm"
    mode :so
    sources "libc/**/*.c", "libc/**/*.S"
    install "sysroot/usr/bin"
    alt_dir "libc_so"
  end

  target "libk.a" do
    language "C", "asm"
    sources [
      "libc/ctype.c",
      "libc/string.c",
      "libc/stdio.c",
      "libc/stdlib.c",
      "libc/malloc.c",
      "libc/errno.c",
      "libc/signal.c",
      "libc/x86_64/nightingale.c",
      "libc/setjmp.S",
    ]
    mode :libk
    alt_dir "libk"
  end

  target "ngk.elf" do
    depends "libk.a"
    language "C", "asm"
    sources [
      "kernel/**/*.c",
      "linker/elf-ng.c",
      "linker/modld.c",
      "fs/**/*.c",
      "x86/**/*.c",
      "x86/**/*.asm",
    ]
    mode :kernel
    link "k", "gcc"
    install "."
  end

  target "crt0.o" do
    mode :crt
    sources "libc/crt0.S"
    install "sysroot/usr/lib"
    alt_dir "crt"
  end

  target "sh" do
    depends "libc.a" ,"crt0.o"
    sources "sh/**/*.c"
    mode :user
    alt_dir "sh-"
    install "sysroot/usr/bin"
  end

  target "ld-ng.so" do
    sources [
      "linker/elf-ng.c",
      "linker/ldso.c",
    ]
    mode :so
    alt_dir "ld-ng"
    install "sysroot/usr/bin"
  end

  Pathname.glob "modules/*.c" do |program_source|
    program = program_source.basename(".c").sub_ext(".ko")
    target program do
      sources program_source
      mode :module
      install "sysroot/usr/bin"
    end
  end

  Pathname.glob "user/*.c" do |program_source|
    program = program_source.basename(".c")
    target program do
      if DYNAMIC
        depends "libc.so", "crt0.o"
      else
        depends "libc.a", "crt0.o"
      end
      sources program_source
      mode :user
      install "sysroot/usr/bin"
    end
  end
end

# require 'yaml'
# puts YAML.dump(build)
puts build.render
