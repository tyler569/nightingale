#pragma once
#ifndef _NIGHTINGALE_H_
#define _NIGHTINGALE_H_

#include <basic.h>
#include <sys/types.h>
#include <stdnoreturn.h>
#include <ng/syscall_consts.h>
#include <ng/cpu.h>
#include <syscall_types.h>

extern const char *syscall_names[];

noreturn int haltvm(int exit_code);

long xtime();

pid_t create(const char *executable);
int procstate(pid_t destination, enum procstate flags);

int fault(enum fault_type type);

#if X86_64
// TODO -> libc/include/x86_64 & include here
enum frame_values {
        ARG0,
        ARG1,
        ARG2,
        ARG3,
        ARG4,
        ARG5,
        ARG6,
        RET_VAL,
        RET_ERR,
        ARGC,
        ARGV,
        ENVP,
};

uintptr_t frame_get(interrupt_frame *, int reg);
uintptr_t frame_set(interrupt_frame *, int reg, uintptr_t value);

void print_registers(interrupt_frame *);
#endif

#endif // _NIGHTINGALE_H_
