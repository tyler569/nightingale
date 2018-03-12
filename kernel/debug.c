
#include <basic.h>
#include <print.h>
#include <panic.h>
#include "debug.h"


int backtrace_from_here(int max_frames) {
    printf("backtrace:\n");

    uintptr_t *rbp;
    rbp = (uintptr_t *)(&rbp - 3);

    backtrace_from((uintptr_t)rbp, max_frames);
    return 0;
}

int bt_test(int x) {
    if (x > 1) {
        return bt_test(x-1) + 1;
    } else {
        return backtrace_from_here(15);
    }
}

int backtrace_from(uintptr_t rbp_, int max_frames) {
    printf("backtrace from %lx:\n", rbp_);

    usize *rbp = (usize *)rbp_;
    usize rip;

    for (int frame=0; frame<max_frames; frame++) {
        if (rbp == 0) // I do still want to print the last (null) frame (atl for now)
            rip = 0;
        else
            rip = rbp[1];

        /* TODO: #ifdef __human_readable_errors */
        printf("  rbp: %#018x   rip: %#018x\n", rbp, rip);
        // unwind:
        if (rbp == 0)  break;
        rbp = (usize *)rbp[0];
    }
    printf("top of stack\n");
    return 0;
}

#ifdef __GNUC__

#define STACK_CHK_GUARD 0x595e9fbd94fda766
__attribute__((used))
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((used))
__noreturn void __stack_chk_fail(void) {
    panic("Stack smashing detected");
    __builtin_unreachable();
}

#endif // __GNUC__

