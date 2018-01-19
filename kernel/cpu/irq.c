
#include <basic.h>
#include <term/print.h>
#include "halt.h"
#include "interrupt.h"
#include "uart.h"
#include "pic.h"

i64 timer_ticks = 0;

void timer_handler(struct interrupt_frame *r) {
    timer_ticks++;
    if (timer_ticks % 1000 == 0) {
        printf("This is tick #%i\n", timer_ticks);
    }
    send_end_of_interrupt(r->interrupt_number - 32);
}

void other_irq_handler(struct interrupt_frame *r) {
    send_end_of_interrupt(r->interrupt_number - 32);
}

