
#pragma once
#ifndef NIGHTINGALE_SYSCALL_H
#define NIGHTINGALE_SYSCALL_H

#include <ng/basic.h>
#include <ng/string.h>
#include <arch/cpu.h>
#include <ng/syscall_consts.h>
#include <stddef.h>
#include <stdint.h>

struct syscall_ret {
        uintptr_t value;
        uintptr_t error;
};

#define RETURN_VALUE(val)                                                      \
        return (struct syscall_ret) { .value = val }
#define RETURN_ERROR(err)                                                      \
        return (struct syscall_ret) { .error = err }

typedef struct syscall_ret sysret;

#define value(V) (sysret) { .value = V }
#define error(E) (sysret) { .error = E }
#define try(expr) ({ sysret _v = expr; if (_v.error) return _v; _v; })

sysret do_syscall(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                uintptr_t arg3, uintptr_t arg4, uintptr_t arg5,
                uintptr_t arg6, interrupt_frame *frame);

sysret do_syscall_with_table(int syscall_num, uintptr_t arg1,
                uintptr_t arg2, uintptr_t arg3,
                uintptr_t arg4, uintptr_t arg5,
                uintptr_t arg6,
                interrupt_frame *frame);

#endif
