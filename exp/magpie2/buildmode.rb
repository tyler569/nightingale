require_relative "./default_compilers.rb"

COMMON_CFLAGS = ["common"]

KERNEL_CFLAGS = [*COMMON_CFLAGS, "kernel", "cflags"]
KERNEL_CXXFLAGS = ["kernel", "cxxflags"]

USER_CFLAGS = [*COMMON_CFLAGS, "user", "cflags"]
USER_CXXFLAGS = ["user", "cxxflags"]
USER_LDFLAGS = ["user", "ldflags"]

kernel_buildmode = {
  cc: gcc_compiler("x86_64-nightingale-gcc", default_flags: KERNEL_CFLAGS),
  cxx: gcc_compiler("x86_64-nightingale-g++", default_flags: KERNEL_CXXFLAGS),
  as: nasm_assembler,
  ld: ld_r_linker("x86_64-nightingale-ld")
}

user_buildmode = {
  cc: gcc_compiler("x86_64-nightingale-gcc", default_flags: USER_CFLAGS),
  cxx: gcc_compiler("x86_64-nightingale-g++", default_flags: USER_CXXFLAGS),
  as: gas_assembler("x86_64-nightingale-gcc"),
  ld: gcc_linker("x86_64-nightingale-gcc", default_flags: USER_LDFLAGS),
}

# p kernel_buildmode[:cc].call(input: "main.c", output: "main.o", dep: "dep/kernel/main.d", flags: ["-Wno-something"])
# p kernel_buildmode[:cxx].call(input: "main.cc", output: "main.o", dep: "dep/kernel/main.d", flags: ["-Wno-something"])
# p kernel_buildmode[:as].call(input: "main.asm", output: "main.o", dep: "dep/kernel/main.d", flags: ["-Wno-something"])
# p kernel_buildmode[:ld].call(input: ["main.o", "lib.o"], output: "main.o", dep: "dep/kernel/main.d", flags: ["-Wno-something"])

# p user_buildmode[:cc].call(input: "main.c", output: "main.o", dep: "dep/user/main.d", flags: ["-Wno-something"])
# p user_buildmode[:cxx].call(input: "main.cc", output: "main.o", dep: "dep/user/main.d", flags: ["-Wno-something"])
# p user_buildmode[:as].call(input: "main.S", output: "main.o", dep: "dep/user/main.d", flags: ["-Wno-something"])
# p user_buildmode[:ld].call(input: ["main.o", "lib.o"], output: "main.o", dep: "dep/user/main.d", flags: ["-Wno-something"])
