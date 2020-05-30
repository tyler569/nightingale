
#pragma once
#ifndef NG_X86_CPU_H
#define NG_X86_CPU_H

#include <basic.h>

#if X86_64
#include "64/cpu.h"
#elif I686
#include "32/cpu.h"
#else
#error "unsupported arch in cpu"
#endif

#define NIRQS 16

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

#define INTERRUPT_ENABLE 0x200

uintptr_t frame_get(interrupt_frame *, int reg);
uintptr_t frame_set(interrupt_frame *, int reg, uintptr_t value);

#endif // NG_X86_CPU_H

