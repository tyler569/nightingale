
#pragma once
#ifndef NG_SYSCALL_H
#define NG_SYSCALL_H

#include <ng/basic.h>
#include <ng/string.h>
#include <ng/cpu.h>
#include <ng/syscall_consts.h>
#include <stddef.h>
#include <stdint.h>

struct syscall_ret {
        intptr_t value;
        intptr_t error;
};

#define RETURN_VALUE(val)                                                      \
        return (struct syscall_ret) { .value = val }
#define RETURN_ERROR(err)                                                      \
        return (struct syscall_ret) { .error = err }

typedef struct syscall_ret sysret;

#define value(V) (sysret) { .value = V }
#define error(E) (sysret) { .error = E }
#define try(expr) ({ sysret _v = expr; if (_v.error) return _v; _v; })

sysret do_syscall(int syscall_num, intptr_t arg1, intptr_t arg2,
                intptr_t arg3, intptr_t arg4, intptr_t arg5,
                intptr_t arg6, interrupt_frame *frame);

sysret do_syscall_with_table(int syscall_num, intptr_t arg1,
                intptr_t arg2, intptr_t arg3,
                intptr_t arg4, intptr_t arg5,
                intptr_t arg6,
                interrupt_frame *frame);

#endif // NG_SYSCALL_H

