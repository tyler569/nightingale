
#pragma once
#ifndef _SETJMP_H_
#define _SETJMP_H_

// #include <basic.h>
#include <stdint.h>
#include <stdnoreturn.h>

#if __x86_64__
union __jmp_buf {
    struct {
        long rbx;
        long bp;
        long r12;
        long r13;
        long r14;
        long r15;
        long sp;
        long ip;
    } __regs;
    long __array[8];
};
#elif __i686__
typedef long __jmp_buf[6];
#else
#error "unimplemeneted"
#endif

typedef union __jmp_buf jmp_buf[1];

int _setjmp(jmp_buf);
int setjmp(jmp_buf);

noreturn void longjmp(jmp_buf, int);
noreturn void _longjmp(jmp_buf, int);

#endif // _SETJMP_H_

