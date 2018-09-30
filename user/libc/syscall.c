
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <errno.h>
#include <ng_syscall.h>
#include "unistd.h"

struct syscall_ret { uintptr_t value, error; };

#if defined(__x86_64__)

struct syscall_ret syscall0(int syscall_num) {
    struct syscall_ret ret;
    asm volatile ("int $0x80 \n\t" : "=a"(ret.value), "=c"(ret.error) :
                  "0"(syscall_num));
    return ret;
}

struct syscall_ret syscall1(int syscall_num, uintptr_t arg1) {
    struct syscall_ret ret;
    asm volatile ("int $0x80 \n\t" : "=a"(ret.value), "=c"(ret.error) :
                  "0"(syscall_num), "D"(arg1));
    return ret;
}

struct syscall_ret syscall2(int syscall_num, uintptr_t arg1, uintptr_t arg2) {
    struct syscall_ret ret;
    asm volatile ("int $0x80 \n\t" : "=a"(ret.value), "=c"(ret.error) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2));
    return ret;
}

struct syscall_ret syscall3(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3) {
    struct syscall_ret ret;
    asm volatile ("int $0x80 \n\t" : "=a"(ret.value), "=c"(ret.error) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3));
    return ret;
}

struct syscall_ret syscall4(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3, uintptr_t arg4) {
    struct syscall_ret ret;
    asm volatile ("int $0x80 \n\t" : "=a"(ret.value), "=c"(ret.error) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3), "1"(arg4));
    return ret;
}

struct syscall_ret syscall5(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3, uintptr_t arg4, uintptr_t arg5) {
    struct syscall_ret ret;
    asm volatile ("mov %7, %%r8\n\t"
                  "int $0x80 \n\t" : "=a"(ret.value), "=c"(ret.error) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3), "1"(arg4),
                  "rm"(arg5));
    return ret;
}

struct syscall_ret syscall6(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3, uintptr_t arg4, uintptr_t arg5,
                            uintptr_t arg6) {
    struct syscall_ret ret;
    asm volatile ("mov %7, %%r8\n\t"
                  "mov %8, %%r9\n\t"
                  "int $0x80 \n\t" : "=a"(ret.value), "=c"(ret.error) :
                  "0"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3), "1"(arg4),
                  "rm"(arg5), "rm"(arg6));
    return ret;
}

#elif defined(__i686__)

struct syscall_ret syscall0(int syscall_num) {
    struct syscall_ret ret;
    asm volatile (
        "int $0x80 \n\t"
        : "=a"(ret.value), "=c"(ret.error)
        : "0"(syscall_num)
    );
    return ret;
}

struct syscall_ret syscall1(int syscall_num, uintptr_t arg1) {
    struct syscall_ret ret;
    asm volatile (
        "push %3 \n\t"
        "int $0x80 \n\t"
        "add $4, %%esp"
        : "=a"(ret.value), "=c"(ret.error)
        : "0"(syscall_num), "rm"(arg1)
    );
    return ret;
}

struct syscall_ret syscall2(int syscall_num, uintptr_t arg1, uintptr_t arg2) {
    struct syscall_ret ret;
    asm volatile (
        "push %4 \n\t"
        "push %3 \n\t"
        "int $0x80 \n\t"
        "add $8, %%esp"
        : "=a"(ret.value), "=c"(ret.error)
        : "0"(syscall_num), "rm"(arg1), "rm"(arg2)
    );
    return ret;
}

struct syscall_ret syscall3(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3) {
    struct syscall_ret ret;
    asm volatile (
        "push %5 \n\t"
        "push %4 \n\t"
        "push %3 \n\t"
        "int $0x80 \n\t"
        "add $12, %%esp"
        : "=a"(ret.value), "=c"(ret.error)
        : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3)
    );
    return ret;
}

struct syscall_ret syscall4(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3, uintptr_t arg4) {
    struct syscall_ret ret;
    asm volatile (
        "push %6 \n\t"
        "push %5 \n\t"
        "push %4 \n\t"
        "push %3 \n\t"
        "int $0x80 \n\t"
        "add $16, %%esp"
        : "=a"(ret.value), "=c"(ret.error)
        : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3), "rm"(arg4)
    );
    return ret;
}

struct syscall_ret syscall5(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3, uintptr_t arg4, uintptr_t arg5) {
    struct syscall_ret ret;
    asm volatile (
        "push %7 \n\t"
        "push %6 \n\t"
        "push %5 \n\t"
        "push %4 \n\t"
        "push %3 \n\t"
        "int $0x80 \n\t"
        "add $20, %%esp"
        : "=a"(ret.value), "=c"(ret.error)
        : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3), "rm"(arg4),
          "rm"(arg5)
    );
    return ret;
}

struct syscall_ret syscall6(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3, uintptr_t arg4, uintptr_t arg5,
                            uintptr_t arg6) {
    struct syscall_ret ret;
    asm volatile (
        "push %8 \n\t"
        "push %7 \n\t"
        "push %6 \n\t"
        "push %5 \n\t"
        "push %4 \n\t"
        "push %3 \n\t"
        "int $0x80 \n\t"
        "add $24, %%esp"
        : "=a"(ret.value), "=c"(ret.error)
        : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3), "rm"(arg4),
          "rm"(arg5), "rm"(arg6)
    );
    return ret;
}

#endif

