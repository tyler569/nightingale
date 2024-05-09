#pragma once

#include <ng/cpu.h>
#include <ng/string.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/syscall_consts.h>

BEGIN_DECLS

#define SYSCALL_TABLE_SIZE 256

typedef intptr_t sysret;

void syscall_entry(int);
void syscall_exit(int);

int syscall_register(int num, const char *name, sysret (*)(), const char *debug,
	unsigned ptr_mask);

sysret do_syscall(uintptr_t a0, uintptr_t a1, uintptr_t a2, uintptr_t a3,
	uintptr_t a4, uintptr_t a5, int syscall_number, interrupt_frame *);

END_DECLS
