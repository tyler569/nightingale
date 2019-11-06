
#pragma once
#ifndef NG_SYSCALL_H
#define NG_SYSCALL_H

#include <basic.h>
#include <ng/string.h>
#include <ng/cpu.h>
#include <ng/syscall_consts.h>
#include <stddef.h>
#include <stdint.h>


typedef intptr_t sysret;

sysret do_syscall(int syscall_num, intptr_t arg1, intptr_t arg2,
                intptr_t arg3, intptr_t arg4, intptr_t arg5,
                intptr_t arg6, interrupt_frame *frame);

sysret do_syscall_with_table(int syscall_num, intptr_t arg1,
                intptr_t arg2, intptr_t arg3,
                intptr_t arg4, intptr_t arg5,
                intptr_t arg6,
                interrupt_frame *frame);

#endif // NG_SYSCALL_H

