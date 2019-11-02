
#pragma once
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <basic.h>
#include <ng/syscall_consts.h>
#include <stdbool.h>
#include <stddef.h>

struct syscall_ret {
        intptr_t value;
        bool is_error;
};

struct syscall_ret syscall0(int syscall_num);
struct syscall_ret syscall1(int syscall_num, intptr_t arg1);
struct syscall_ret syscall2(int syscall_num, intptr_t arg1, intptr_t arg2);
struct syscall_ret syscall3(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3);
struct syscall_ret syscall4(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3, intptr_t arg4);
struct syscall_ret syscall5(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3, intptr_t arg4, intptr_t arg5);
struct syscall_ret syscall6(int syscall_num, intptr_t arg1, intptr_t arg2,
                            intptr_t arg3, intptr_t arg4, intptr_t arg5,
                            intptr_t arg6);

#endif // _SYSCALL_H_

