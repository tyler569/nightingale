#pragma once
#ifndef _NIGHTINGALE_H_
#define _NIGHTINGALE_H_

#include <ng/cpu.h>
#include <ng/syscall_consts.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <syscall_types.h>

BEGIN_DECLS

extern const char *syscall_names[];

_Noreturn int haltvm(int exit_code);
long xtime();
pid_t create(const char *executable);
int procstate(pid_t destination, enum procstate flags);
int fault(enum fault_type type);
void print_registers(interrupt_frame *);
int syscall_trace(pid_t pid, int state);
int top(int show_threads);
int load_module(int fd);
int sleepms(int milliseconds);

void redirect_output_to(char *const argv[]);

int __ng_test_args(int, int, int, int, int, int);

END_DECLS

#endif // _NIGHTINGALE_H_
