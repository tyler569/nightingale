
#pragma once
#ifndef NIGHTINGALE_ARCH_X86_CPU_H
#define NIGHTINGALE_ARCH_X86_CPU_H

#include <basic.h>

#if defined(__x86_64__)
#include "cpu64.h"
#elif defined(__i686__)
#include "cpu32.h"
#else
#error "unsupported arch in cpu"
#endif

typedef uint16_t port_addr_t;

uint8_t inb(port_addr_t port);
void outb(port_addr_t port, uint8_t data);
uint16_t inw(port_addr_t port);
void outw(port_addr_t port, uint16_t data);
uint32_t ind(port_addr_t port);
void outd(port_addr_t port, uint32_t data);

uint64_t rdtsc();

void set_vm_root(uintptr_t);
void invlpg(uintptr_t);
void flush_tlb(void);

uint64_t rdmsr(uint32_t msr_id);
void wrmsr(uint32_t msr_id, uint64_t value);

/*
 * reg:
 */

enum {
    SP, BP, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6,
    RET_VAL, RET_ERR, FLAGS, ARGC, ARGV, ENVP, USER_SP,
    IP,
};

#if defined(__x86_64__) || defined(__i686__)
#define INTERRUPT_ENABLE 0x200
#endif

uintptr_t frame_get(interrupt_frame*, int reg);
uintptr_t frame_set(interrupt_frame*, int reg, uintptr_t value);

#endif

