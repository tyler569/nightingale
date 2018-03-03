
#pragma once
#ifndef NIGHTINGALE_X86_CPU_H
#define NIGHTINGALE_X86_CPU_H

#include <basic.h>

typedef struct interrupt_frame {
    u64 r15, r14, r13, r12, r11, r10, r9, r8;
    u64 rbp, rdi, rsi, rdx, rbx, rcx, rax;
    u64 interrupt_number, error_code;
    u64 rip, cs, rflags, user_rsp, ss;
} interrupt_frame;

typedef u16 port_addr_t;

u8 inb(port_addr_t port);
void outb(port_addr_t port, u8 data);
u16 inw(port_addr_t port);
void outw(port_addr_t port, u16 data);
u32 ind(port_addr_t port);
void outd(port_addr_t port, u32 data);

u64 rdtsc();

u64 rdmsr(u32 msr_id);
void wrmsr(u32 msr_id, u64 value);

#endif

