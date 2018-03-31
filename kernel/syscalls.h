
#pragma once 
#ifndef NIGHTINGALE_SYSCALLS_H
#define NIGHTINGALE_SYSCALLS_H

#include <stddef.h>
#include <stdint.h>
#include <basic.h>
#include "syscall.h"

struct syscall_ret sys_exit(int exit_status);

#endif

