
#include <errno.h>
#include <ng/syscall_consts.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include "unistd.h"

struct syscall_ret {
        intptr_t value, is_error;
};

#if defined(__x86_64__)

#define VALUE "=a"(ret.value)
#define ERROR "=@ccc"(ret.is_error)
#define SYSCN "0"(syscall_num)
#define ARG1 "D"(arg1)
#define ARG2 "S"(arg2)
#define ARG3 "d"(arg3)
#define ARG4 "c"(arg4)
#define ARG5 "rm"(arg5)
#define ARG6 "rm"(arg6)

struct syscall_ret syscall0(int syscall_num) {
        struct syscall_ret ret;
        asm volatile("int $0x80 \n\t" : VALUE, ERROR : SYSCN);
        return ret;
}

struct syscall_ret syscall1(int syscall_num, intptr_t arg1) {
        struct syscall_ret ret;
        asm volatile("int $0x80 \n\t" : VALUE, ERROR : SYSCN, ARG1);
        return ret;
}

struct syscall_ret syscall2(int syscall_num, intptr_t arg1, intptr_t arg2) {
        struct syscall_ret ret;
        asm volatile("int $0x80 \n\t" : VALUE, ERROR : SYSCN, ARG1, ARG2);
        return ret;
}

struct syscall_ret syscall3(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3) {
        struct syscall_ret ret;
        asm volatile("int $0x80 \n\t" : VALUE, ERROR : SYSCN, ARG1, ARG2, ARG3);
        return ret;
}

struct syscall_ret syscall4(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3, intptr_t arg4) {
        struct syscall_ret ret;
        asm volatile("int $0x80 \n\t"
                     : VALUE, ERROR
                     : SYSCN, ARG1, ARG2, ARG3, ARG4);
        return ret;
}

struct syscall_ret syscall5(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3, intptr_t arg4, intptr_t arg5) {
        struct syscall_ret ret;
        asm volatile("mov %7, %%r8\n\t"
                     "int $0x80 \n\t"
                     : VALUE, ERROR
                     : SYSCN, ARG1, ARG2, ARG3, ARG4, ARG5);
        return ret;
}

struct syscall_ret syscall6(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3, intptr_t arg4, intptr_t arg5,
                            intptr_t arg6) {
        struct syscall_ret ret;
        asm volatile("mov %7, %%r8\n\t"
                     "mov %8, %%r9\n\t"
                     "int $0x80 \n\t"
                     : VALUE, ERROR
                     : SYSCN, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
        return ret;
}

#elif defined(__i686__)

struct syscall_ret syscall0(int syscall_num) {
        struct syscall_ret ret;
        asm volatile("int $0x80 \n\t"
                     : "=a"(ret.value), "=@ccc"(ret.is_error)
                     : "0"(syscall_num));
        return ret;
}

struct syscall_ret syscall1(int syscall_num, intptr_t arg1) {
        struct syscall_ret ret;
        asm volatile("push %3 \n\t"
                     "int $0x80 \n\t"
                     "add $4, %%esp"
                     : "=a"(ret.value), "=@ccc"(ret.is_error)
                     : "0"(syscall_num), "rm"(arg1));
        return ret;
}

struct syscall_ret syscall2(int syscall_num, intptr_t arg1, intptr_t arg2) {
        struct syscall_ret ret;
        asm volatile("push %4 \n\t"
                     "push %3 \n\t"
                     "int $0x80 \n\t"
                     "add $8, %%esp"
                     : "=a"(ret.value), "=@ccc"(ret.is_error)
                     : "0"(syscall_num), "rm"(arg1), "rm"(arg2));
        return ret;
}

struct syscall_ret syscall3(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3) {
        struct syscall_ret ret;
        asm volatile("push %5 \n\t"
                     "push %4 \n\t"
                     "push %3 \n\t"
                     "int $0x80 \n\t"
                     "add $12, %%esp"
                     : "=a"(ret.value), "=@ccc"(ret.is_error)
                     : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3));
        return ret;
}

struct syscall_ret syscall4(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3, intptr_t arg4) {
        struct syscall_ret ret;
        asm volatile("push %6 \n\t"
                     "push %5 \n\t"
                     "push %4 \n\t"
                     "push %3 \n\t"
                     "int $0x80 \n\t"
                     "add $16, %%esp"
                     : "=a"(ret.value), "=@ccc"(ret.is_error)
                     : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3),
                       "rm"(arg4));
        return ret;
}

struct syscall_ret syscall5(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3, intptr_t arg4, intptr_t arg5) {
        struct syscall_ret ret;
        asm volatile("push %7 \n\t"
                     "push %6 \n\t"
                     "push %5 \n\t"
                     "push %4 \n\t"
                     "push %3 \n\t"
                     "int $0x80 \n\t"
                     "add $20, %%esp"
                     : "=a"(ret.value), "=@ccc"(ret.is_error)
                     : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3),
                       "rm"(arg4), "rm"(arg5));
        return ret;
}

struct syscall_ret syscall6(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3, intptr_t arg4, intptr_t arg5,
                            intptr_t arg6) {
        struct syscall_ret ret;
        asm volatile("push %8 \n\t"
                     "push %7 \n\t"
                     "push %6 \n\t"
                     "push %5 \n\t"
                     "push %4 \n\t"
                     "push %3 \n\t"
                     "int $0x80 \n\t"
                     "add $24, %%esp"
                     : "=a"(ret.value), "=@ccc"(ret.is_error)
                     : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3),
                       "rm"(arg4), "rm"(arg5), "rm"(arg6));
        return ret;
}

#endif
