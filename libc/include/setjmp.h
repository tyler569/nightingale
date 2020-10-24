#pragma once
#ifndef _SETJMP_H_
#define _SETJMP_H_

#include <basic.h>
#include <stdnoreturn.h>

#if X86_64
union __jmp_buf {
        struct {
                unsigned long rbx;
                unsigned long bp;
                unsigned long r12;
                unsigned long r13;
                unsigned long r14;
                unsigned long r15;
                unsigned long sp;
                unsigned long ip;
        } __regs;
        unsigned long __array[8];
};
#endif

typedef union __jmp_buf jmp_buf[1];

__RETURNS_TWICE
int _setjmp(jmp_buf);
__RETURNS_TWICE
int setjmp(jmp_buf);

noreturn void longjmp(jmp_buf, int);
noreturn void _longjmp(jmp_buf, int);

#endif // _SETJMP_H_
