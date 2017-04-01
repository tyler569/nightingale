
#include <stdio.h>
#include "../halt.h"
#include "interrupt.h"
#include "../drivers/8250uart.h"
#include "../drivers/8259pic.h"

long timer_ticks = 0;

void timer_handler(struct interrupt_frame *r) {
    timer_ticks++;
    if (timer_ticks % 200 == 0) {
        printf("This is tick #%i\n", timer_ticks);
    }
    return;
}

void keyboard_handler(struct interrupt_frame *r) {
    return;
}

void uart_handler(struct interrupt_frame *r) {
    char f = uart_read_byte(com1.base);
    com1.write(&f, 1);
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
        case 0x24:
            uart_handler(r);
            break;
        default:
            break;
    }
    // Acknowlege IRQ with he PIC
    send_end_of_interrupt(r->interrupt_number - 32);
}

