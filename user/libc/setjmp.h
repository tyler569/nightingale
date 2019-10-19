
#ifndef _SETJMP_H_
#define _SETJMP_H_

#include <basic.h>
#include <stdint.h>

#if X86_64
typedef long __jmp_buf[8];
#elif I686
typedef long __jmp_buf[6];
#else
#error "unimplemeneted"
#endif

typedef struct __jmp_buf_tag {
        __jmp_buf __jb;
        unsigned long __fl;
        unsigned long __ss[128 / sizeof(long)];
} jmp_buf[1];

int _setjmp(jmp_buf);
int setjmp(jmp_buf);

noreturn void longjmp(jmp_buf, int);
noreturn void _longjmp(jmp_buf, int);

#endif

