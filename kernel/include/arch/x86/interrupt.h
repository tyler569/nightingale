
#pragma once
#ifndef NIGHTINGALE_INTERRUPT_H
#define NIGHTINGALE_INTERRUPT_H

#include <basic.h>

typedef struct interrupt_frame {
    u64 r15, r14, r13, r12, r11, r10, r9, r8;
    u64 rdi, rsi, rbp, /*rsp,*/ rbx, rdx, rcx, rax;
    u64 interrupt_number, error_code;
    u64 rip, cs, rflags, user_rsp, ss;
} interrupt_frame;

void enable_irqs();
void disable_irqs();

void c_interrupt_shim(interrupt_frame *r);

void divide_by_zero_exception(interrupt_frame *r);
void page_fault(interrupt_frame *r);
void gp_exception(interrupt_frame *r);
void panic_exception(interrupt_frame *r);
void generic_exception(interrupt_frame *r);

void timer_handler(interrupt_frame *r);
void keyboard_handler(interrupt_frame *r);
void other_irq_handler(interrupt_frame *r);

void syscall_handler(interrupt_frame *r);


void print_registers(interrupt_frame *r);

#endif
