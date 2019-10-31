
#pragma once
#ifndef NIGHTINGALE_ARCH_X86_CPU64_H
#define NIGHTINGALE_ARCH_X86_CPU64_H

#include <ng/basic.h>

typedef struct interrupt_frame {
        uint64_t ds;
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t rbp, rdi, rsi, rdx, rbx, rcx, rax;
        uint64_t interrupt_number, error_code;
        uint64_t rip, cs, rflags, user_rsp, ss;
} interrupt_frame;

void print_registers(interrupt_frame *);

#endif
