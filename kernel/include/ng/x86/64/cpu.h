#pragma once
#ifndef NG_X86_64_CPU_H
#define NG_X86_64_CPU_H

#include <basic.h>

typedef struct interrupt_frame {
        uint64_t ds;
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t bp, rdi, rsi, rdx, rbx, rcx, rax;
        uint64_t interrupt_number, error_code;
        uint64_t ip, cs, flags, user_sp, ss;
} interrupt_frame;

void print_registers(interrupt_frame *);

#endif // NG_X86_64_CPU_H
