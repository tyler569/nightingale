
#include <stdio.h>
#include "../halt.h"
#include "interrupt.h"

__attribute__((interrupt))
void divide_by_zero_exception(struct interrupt_frame *r) {
    printf("Divide by 0 error - halting");
    halt();
}

__attribute__((interrupt))
void general_protection_exception(struct interrupt_frame *r, unsigned long arg) {
    printf("General Protection fault");
    return;
}


__attribute__((interrupt))
void int_20_handler(struct interrupt_frame *r) {
    printf("You fired int 0x20");
    return;
}

void load_idt_gates() {
    load_idt_gate(0x00, divide_by_zero_exception);
    load_idt_gate(0x0D, general_protection_exception);
    load_idt_gate(0x20, int_20_handler);
}
