#pragma once
#ifndef NG_X86_CPU_H
#define NG_X86_CPU_H

#include <basic.h>

#define NIRQS 16

typedef struct interrupt_frame {
        uint64_t ds;
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t bp, rdi, rsi, rdx, rbx, rcx, rax;
        uint64_t interrupt_number, error_code;
        uint64_t ip, cs, flags, user_sp, ss;
} interrupt_frame;

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

#define INTERRUPT_ENABLE 0x200

#endif // NG_X86_CPU_H
