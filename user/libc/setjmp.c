
#include <setjmp.h>

void longjmp(jmp_buf env, int status) {
        __builtin_longjmp(env, 1);
}

