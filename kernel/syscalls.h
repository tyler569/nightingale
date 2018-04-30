
#pragma once 
#ifndef NIGHTINGALE_SYSCALLS_H
#define NIGHTINGALE_SYSCALLS_H

#include <stddef.h>
#include <stdint.h>
#include <basic.h>
#include "syscall.h"

struct syscall_ret sys_exit(int exit_status);
struct syscall_ret sys_read(int fd, void *buf, size_t len);
struct syscall_ret sys_write(int fd, const void *buf, size_t len);
struct syscall_ret sys_fork(void);
struct syscall_ret sys_top(void);

#endif

