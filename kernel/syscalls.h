
#pragma once 
#ifndef NIGHTINGALE_SYSCALLS_H
#define NIGHTINGALE_SYSCALLS_H

#include <stddef.h>
#include <stdint.h>
#include <basic.h>
#include <arch/x86/cpu.h>
#include "syscall.h"

struct syscall_ret sys_exit(int exit_status);
struct syscall_ret sys_read(int fd, void *buf, size_t len);
struct syscall_ret sys_write(int fd, const void *buf, size_t len);
struct syscall_ret sys_fork(interrupt_frame *frame);
struct syscall_ret sys_top(void);
struct syscall_ret sys_getpid(void);
struct syscall_ret sys_gettid(void);

#endif

