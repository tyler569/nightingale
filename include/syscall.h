#pragma once
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <basic.h>
#include <stddef.h>
#include <stdint.h>

intptr_t syscall0(int syscall_num);

intptr_t syscall1(int syscall_num, intptr_t arg1);

intptr_t syscall2(int syscall_num, intptr_t arg1, intptr_t arg2);

intptr_t syscall3(int syscall_num, intptr_t arg1, intptr_t arg2, intptr_t arg3);

intptr_t syscall4(
    int syscall_num,
    intptr_t arg1,
    intptr_t arg2,
    intptr_t arg3,
    intptr_t arg4
);

intptr_t syscall5(
    int syscall_num,
    intptr_t arg1,
    intptr_t arg2,
    intptr_t arg3,
    intptr_t arg4,
    intptr_t arg5
);

intptr_t syscall6(
    int syscall_num,
    intptr_t arg1,
    intptr_t arg2,
    intptr_t arg3,
    intptr_t arg4,
    intptr_t arg5,
    intptr_t arg6
);

#endif // _SYSCALL_H_
