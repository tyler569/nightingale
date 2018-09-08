
#pragma once 
#ifndef NIGHTINGALE_SYSCALLS_H
#define NIGHTINGALE_SYSCALLS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <basic.h>
#include <arch/x86/cpu.h>
#include "thread.h"
#include "syscall.h"

struct syscall_ret sys_exit(int exit_status);
struct syscall_ret sys_read(int fd, void* buf, size_t len);
struct syscall_ret sys_write(int fd, void const* buf, size_t len);
struct syscall_ret sys_fork(interrupt_frame* frame);
struct syscall_ret sys_top(void);
struct syscall_ret sys_getpid(void);
struct syscall_ret sys_gettid(void);
struct syscall_ret sys_execve(interrupt_frame* frame, char* file, char** argv, char** envp);
struct syscall_ret sys_wait4(pid_t);
struct syscall_ret sys_socket(int, int, int);
struct syscall_ret sys_bind0(int, uint32_t, size_t);
struct syscall_ret sys_connect0(int, uint32_t, uint16_t);
struct syscall_ret sys_strace(bool);

#endif

