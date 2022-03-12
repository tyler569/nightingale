#pragma once
#ifndef NG_SYSCALL_H
#define NG_SYSCALL_H

#include <basic.h>
#include <ng/cpu.h>
#include <ng/string.h>
#include <ng/syscall_consts.h>
#include <stddef.h>
#include <stdint.h>

#define SYSCALL_TABLE_SIZE 256

typedef intptr_t sysret;

void syscall_entry(int);
void syscall_exit(int);

int syscall_register(
    int num,
    const char *name,
    sysret (*)(),
    const char *debug,
    unsigned ptr_mask
);

sysret do_syscall(interrupt_frame *);

#endif // NG_SYSCALL_H
