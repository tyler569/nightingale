
#pragma once
#ifndef NIGHTINGALE_SYSCALL_H
#define NIGHTINGALE_SYSCALL_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if defined(__x86_64__)
#include <arch/x86/cpu64.h>
#elif defined(__i686__)
#include <arch/x86/cpu32.h>
#else
#error "unsupported machine at syscall"
#endif

#include <ng_syscall.h>

struct syscall_ret {
    uintptr_t value;
    uintptr_t error;
};

#define RETURN_VALUE(val) \
    do { \
        struct syscall_ret _x_ret = { val, 0 }; \
        return _x_ret; \
    } while (0)
#define RETURN_ERROR(err) \
    do { \
        struct syscall_ret _x_ret = { 0, err }; \
        return _x_ret; \
    } while (0)

struct syscall_ret do_syscall(int syscall_num,
        uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
        uintptr_t arg4, uintptr_t arg5, uintptr_t arg6,
        interrupt_frame *frame);

struct syscall_ret do_syscall_with_table(int syscall_num,
        uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
        uintptr_t arg4, uintptr_t arg5, uintptr_t arg6,
        interrupt_frame *frame);

#endif

