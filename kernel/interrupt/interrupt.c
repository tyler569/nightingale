
#include <stdio.h>
#include "../halt.h"
#include "interrupt.h"
#include "irq.h"

void divide_by_zero_exception(struct interrupt_frame *r) {
    printf("Divide by 0 error - halting\n");
    disable_irqs();
    halt();
}

void general_protection_exception(struct interrupt_frame *r) {
    printf("General Protection fault\n");
    printf("Error code: 0x%x\n", r->error_code);
    return;
}

void generic_exception(struct interrupt_frame *r) {
    printf("Unhandled exception at 0x%x\n", r->rip);
    printf("Exception: 0x%x Error code: 0x%x", r->interrupt_number, r->error_code);
    halt();
}

