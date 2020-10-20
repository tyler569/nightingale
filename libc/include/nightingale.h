
#pragma once
#ifndef _NIGHTINGALE_H_
#define _NIGHTINGALE_H_

#include <ng/syscall_consts.h>
#include <ng/x86/cpu.h>
#include <syscall_types.h>

extern const char *syscall_names[];

noreturn int haltvm(int exit_code);

long xtime();

pid_t create(const char *executable);
int procstate(pid_t destination, enum procstate flags);

int fault(enum fault_type type);

#endif // _NIGHTINGALE_H_

