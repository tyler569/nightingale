
#include <stdio.h>
#include "halt.h"
#include "interrupt.h"
#include "uart.h"
#include "pic.h"

long timer_ticks = 0;

void timer_handler(struct interrupt_frame *r) {
    timer_ticks++;
    if (timer_ticks % 200 == 0) {
        printf("This is tick #%i\n", timer_ticks);
    }
    send_end_of_interrupt(r->interrupt_number - 32);
}

void other_irq_handler(struct interrupt_frame *r) {
    send_end_of_interrupt(r->interrupt_number - 32);
}

