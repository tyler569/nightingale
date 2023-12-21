#pragma once
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

static inline intptr_t __syscall6(int num, intptr_t a1, intptr_t a2,
    intptr_t a3, intptr_t a4, intptr_t a5, intptr_t a6)
{
    intptr_t s_ret;
    intptr_t s_err;
    register intptr_t snum = (intptr_t)(num);
    register intptr_t sa1 = (intptr_t)(a1);
    register intptr_t sa2 = (intptr_t)(a2);
    register intptr_t sa3 = (intptr_t)(a3);
    register intptr_t sa4 = (intptr_t)(a4);
    register intptr_t sa5 __asm__("r8") = (intptr_t)(a5);
    register intptr_t sa6 __asm__("r9") = (intptr_t)(a6);
    __asm__ volatile(
        "int $0x80"
        : "=a"(s_ret), "=d"(s_err)
        : "0"(snum), "D"(sa1), "S"(sa2), "d"(sa3), "c"(sa4), "r"(sa5), "r"(sa6)
        : "memory");
    return s_ret;
}

#define __syscall0(num) __syscall6(num, 0, 0, 0, 0, 0, 0)
#define __syscall1(num, a1) __syscall6(num, a1, 0, 0, 0, 0, 0)
#define __syscall2(num, a1, a2) __syscall6(num, a1, a2, 0, 0, 0, 0)
#define __syscall3(num, a1, a2, a3) __syscall6(num, a1, a2, a3, 0, 0, 0)
#define __syscall4(num, a1, a2, a3, a4) __syscall6(num, a1, a2, a3, a4, 0, 0)
#define __syscall5(num, a1, a2, a3, a4, a5) \
    __syscall6(num, a1, a2, a3, a4, a5, 0)

#endif // _SYSCALL_H_
