
#pragma once

struct interrupt_frame {
    unsigned long r15, r14, r13, r12, r11, r10, r9, r8;
    unsigned long rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
    unsigned long interrupt_number, error_code;
    unsigned long rip, cs, rflags, user_rsp, ss;
};

