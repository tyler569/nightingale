
#include <stdio.h>
#include "../halt.h"
#include "interrupt.h"

void divide_by_zero_exception(struct interrupt_frame *r) {
    printf("Divide by 0 error - halting\n");
    halt();
}

void general_protection_exception(struct interrupt_frame *r) {
    printf("General Protection fault\n");
    printf("Error code: 0x%x\n", r->error_code);
    return;
}

void int_20_handler(struct interrupt_frame *r) {
    printf("You fired int 0x20");
    return;
}

void unhandled_exception(struct interrupt_frame *r) {
    printf("Unhandled exception at 0x%x\n", r->rip);
    printf("Exception: 0x%x Error code: 0x%x", r->interrupt_number, r->error_code);
    halt();
}

void c_interrupt_shim(struct interrupt_frame *r) {
    switch (r->interrupt_number) {
        case 0x0:
            divide_by_zero_exception(r);
            break;
        case 0xD:
            general_protection_exception(r);
            break;
        case 0x20:
            int_20_handler(r);
            break;
        default:
            unhandled_exception(r);
    }
}
