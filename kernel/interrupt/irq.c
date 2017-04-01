
#include <stdio.h>
#include "../halt.h"
#include "interrupt.h"
#include "../drivers/8259pic.h"

long timer_ticks = 0;

void timer_handler(struct interrupt_frame *r) {
    timer_ticks++;
    if (timer_ticks % 100 == 0) {
        printf("One second has elapsed (more or less)\n");
    }
    return;
}

void keyboard_handler(struct interrupt_frame *r) {
    return;
}

void c_irq_shim(struct interrupt_frame *r) {
    switch (r->interrupt_number) {
        case 0x20:
            timer_handler(r);
            break;
        case 0x21:
            keyboard_handler(r);
            break;
        default:
            break;
    }
    // Acknowlege IRQ with he PIC
    send_end_of_interrupt(r->interrupt_number - 32);
}

