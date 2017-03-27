
#pragma once

void load_idt_gate(unsigned long gate, void *handler);

struct interrupt_frame {
    unsigned long rip, cs, rflags, rsp, ss;
};

void divide_by_zero_exception(/*something*/);


