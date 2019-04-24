
#ifndef _SETJMP_H_
#define _SETJMP_H_

#include <stdint.h>

typedef uintptr_t jmp_buf[8];

#define setjmp(env) __builtin_setjmp(env)
void longjmp(jmp_buf env, int status);

#endif
