
#pragma once
#ifndef NIGHTINGALE_SYSCALL_H
#define NIGHTINGALE_SYSCALL_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <arch/x86/cpu.h>

#include <ng_syscall.h>

struct syscall_ret {
    uintptr_t value;
    uintptr_t error;
};

struct syscall_ret do_syscall(int syscall_num,
        uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
        uintptr_t arg4, uintptr_t arg5, uintptr_t arg6,
        interrupt_frame *frame);

struct syscall_ret do_syscall_with_table(int syscall_num,
        uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
        uintptr_t arg4, uintptr_t arg5, uintptr_t arg6,
        interrupt_frame *frame);

#endif

