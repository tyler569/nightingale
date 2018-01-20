
#pragma once
#ifndef NIGHTINGALE_INTERRUPT_H
#define NIGHTINGALE_INTERRUPT_H

#include <basic.h>

typedef struct interrupt_frame {
    u64 r15, r14, r13, r12, r11, r10, r9, r8;
    u64 rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
    u64 interrupt_number, error_code;
    u64 rip, cs, rflags, user_rsp, ss;
} interrupt_frame;

void enable_irqs();
void disable_irqs();

#endif
