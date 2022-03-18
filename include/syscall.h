#pragma once
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <basic.h>
#include <stddef.h>
#include <stdint.h>

intptr_t __syscall0(int syscall_num);

intptr_t __syscall1(int syscall_num, intptr_t arg1);

intptr_t __syscall2(int syscall_num, intptr_t arg1, intptr_t arg2);

intptr_t __syscall3(
    int syscall_num, intptr_t arg1, intptr_t arg2, intptr_t arg3);

intptr_t __syscall4(int syscall_num, intptr_t arg1, intptr_t arg2,
    intptr_t arg3, intptr_t arg4);

intptr_t __syscall5(int syscall_num, intptr_t arg1, intptr_t arg2,
    intptr_t arg3, intptr_t arg4, intptr_t arg5);

intptr_t __syscall6(int syscall_num, intptr_t arg1, intptr_t arg2,
    intptr_t arg3, intptr_t arg4, intptr_t arg5, intptr_t arg6);

#endif // _SYSCALL_H_
