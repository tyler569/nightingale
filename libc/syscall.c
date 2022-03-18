#include <ng/syscall_consts.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <errno.h>
#include "unistd.h"

#if defined(__x86_64__)

#define CLOBBER "memory"
#define VALUE "=a"(ret)
//#define ERROR "=@ccc"(ret.is_error)
#define SYSCN "0"(syscall_num)
#define ARG1 "D"(arg1)
#define ARG2 "S"(arg2)
#define ARG3 "d"(arg3)
#define ARG4 "c"(arg4)
#define ARG5 "rm"(arg5)
#define ARG6 "rm"(arg6)

intptr_t __syscall0(int syscall_num)
{
    intptr_t ret;
    asm volatile("int $0x80 \n\t" : VALUE:SYSCN : CLOBBER);
    return ret;
}

intptr_t __syscall1(int syscall_num, intptr_t arg1)
{
    intptr_t ret;
    asm volatile("int $0x80 \n\t" : VALUE : SYSCN, ARG1 : CLOBBER);
    return ret;
}

intptr_t __syscall2(int syscall_num, intptr_t arg1, intptr_t arg2)
{
    intptr_t ret;
    asm volatile("int $0x80 \n\t" : VALUE : SYSCN, ARG1, ARG2 : CLOBBER);
    return ret;
}

intptr_t __syscall3(
    int syscall_num, intptr_t arg1, intptr_t arg2, intptr_t arg3)
{
    intptr_t ret;
    asm volatile("int $0x80 \n\t" : VALUE : SYSCN, ARG1, ARG2, ARG3 : CLOBBER);
    return ret;
}

intptr_t __syscall4(
    int syscall_num, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4)
{
    intptr_t ret;
    asm volatile("int $0x80 \n\t"
                 : VALUE
                 : SYSCN, ARG1, ARG2, ARG3, ARG4
                 : CLOBBER);
    return ret;
}

intptr_t __syscall5(int syscall_num, intptr_t arg1, intptr_t arg2,
    intptr_t arg3, intptr_t arg4, intptr_t arg5)
{
    intptr_t ret;
    asm volatile("mov %6, %%r8\n\t"
                 "int $0x80 \n\t"
                 : VALUE
                 : SYSCN, ARG1, ARG2, ARG3, ARG4, ARG5
                 : "r8", CLOBBER);
    return ret;
}

intptr_t __syscall6(int syscall_num, intptr_t arg1, intptr_t arg2,
    intptr_t arg3, intptr_t arg4, intptr_t arg5, intptr_t arg6)
{
    intptr_t ret;
    asm volatile("mov %6, %%r8\n\t"
                 "mov %7, %%r9\n\t"
                 "int $0x80 \n\t"
                 : VALUE
                 : SYSCN, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6
                 : "r8", "r9", CLOBBER);
    return ret;
}

#elif defined(__i686__)

intptr_t __syscall0(int syscall_num)
{
    intptr_t ret;
    asm volatile("int $0x80 \n\t" : "=a"(ret) : "0"(syscall_num));
    return ret;
}

intptr_t __syscall1(int syscall_num, intptr_t arg1)
{
    intptr_t ret;
    asm volatile("push %2 \n\t"
                 "int $0x80 \n\t"
                 "add $4, %%esp"
                 : "=a"(ret)
                 : "0"(syscall_num), "rm"(arg1));
    return ret;
}

intptr_t __syscall2(int syscall_num, intptr_t arg1, intptr_t arg2)
{
    intptr_t ret;
    asm volatile("push %3 \n\t"
                 "push %2 \n\t"
                 "int $0x80 \n\t"
                 "add $8, %%esp"
                 : "=a"(ret)
                 : "0"(syscall_num), "rm"(arg1), "rm"(arg2));
    return ret;
}

intptr_t __syscall3(
    int syscall_num, intptr_t arg1, intptr_t arg2, intptr_t arg3)
{
    intptr_t ret;
    asm volatile("push %4 \n\t"
                 "push %3 \n\t"
                 "push %2 \n\t"
                 "int $0x80 \n\t"
                 "add $12, %%esp"
                 : "=a"(ret)
                 : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3));
    return ret;
}

intptr_t __syscall4(
    int syscall_num, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4)
{
    intptr_t ret;
    asm volatile(
        "push %5 \n\t"
        "push %4 \n\t"
        "push %3 \n\t"
        "push %2 \n\t"
        "int $0x80 \n\t"
        "add $16, %%esp"
        : "=a"(ret)
        : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3), "rm"(arg4));
    return ret;
}

intptr_t __syscall5(int syscall_num, intptr_t arg1, intptr_t arg2,
    intptr_t arg3, intptr_t arg4, intptr_t arg5)
{
    intptr_t ret;
    asm volatile("push %6 \n\t"
                 "push %5 \n\t"
                 "push %4 \n\t"
                 "push %3 \n\t"
                 "push %2 \n\t"
                 "int $0x80 \n\t"
                 "add $20, %%esp"
                 : "=a"(ret)
                 : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3),
                 "rm"(arg4), "rm"(arg5));
    return ret;
}

intptr_t __syscall6(int syscall_num, intptr_t arg1, intptr_t arg2,
    intptr_t arg3, intptr_t arg4, intptr_t arg5, intptr_t arg6)
{
    intptr_t ret;
    asm volatile("push %7 \n\t"
                 "push %6 \n\t"
                 "push %5 \n\t"
                 "push %4 \n\t"
                 "push %3 \n\t"
                 "push %2 \n\t"
                 "int $0x80 \n\t"
                 "add $24, %%esp"
                 : "=a"(ret)
                 : "0"(syscall_num), "rm"(arg1), "rm"(arg2), "rm"(arg3),
                 "rm"(arg4), "rm"(arg5), "rm"(arg6));
    return ret;
}

#endif
