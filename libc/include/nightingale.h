#pragma once
#ifndef _NIGHTINGALE_H_
#define _NIGHTINGALE_H_

#include <basic.h>
#include <ng/cpu.h>
#include <ng/syscall_consts.h>
#include <stdnoreturn.h>
#include <sys/types.h>
#include <syscall_types.h>

extern const char *syscall_names[];

noreturn int haltvm(int exit_code);
long xtime();
pid_t create(const char *executable);
int procstate(pid_t destination, enum procstate flags);
int fault(enum fault_type type);

#if X86_64

void print_registers(interrupt_frame *);

#endif

int strace(int enable);
int top(int show_threads);
int load_module(int fd);
int sleepms(int milliseconds);

void redirect_output_to(char *const argv[]);

#endif // _NIGHTINGALE_H_
