
#pragma once
#ifndef NIGHTINGALE_SYSCALL_H
#define NIGHTINGALE_SYSCALL_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define SYS_INVALID 0
#define SYS_DEBUGPRINT 1
#define SYS_EXIT 2

struct syscall_ret {
    uintptr_t value;
    uintptr_t error;
};

struct syscall_ret do_syscall(int syscall_num,
        uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
        uintptr_t arg4, uintptr_t arg5, uintptr_t arg6);

#endif

