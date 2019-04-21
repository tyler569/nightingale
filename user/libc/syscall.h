
#ifndef NIGHTINGALE_LIBC_SYSCALL_H
#define NIGHTINGALE_LIBC_SYSCALL_H

#include <ng/basic.h>
#include <stddef.h>
#include <stdbool.h>
#include <ng_syscall.h>

struct syscall_ret { uintptr_t value; bool is_error; };

struct syscall_ret syscall0(int syscall_num);
struct syscall_ret syscall1(int syscall_num, uintptr_t arg1);
struct syscall_ret syscall2(int syscall_num, uintptr_t arg1, uintptr_t arg2);
struct syscall_ret syscall3(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3);
struct syscall_ret syscall4(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3, uintptr_t arg4);
struct syscall_ret syscall5(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3, uintptr_t arg4, uintptr_t arg5);
struct syscall_ret syscall6(int syscall_num, uintptr_t arg1, uintptr_t arg2,
                            uintptr_t arg3, uintptr_t arg4, uintptr_t arg5,
                            uintptr_t arg6);

#endif

