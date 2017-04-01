
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "multiboot2.h"
#include "halt.h"
#include "video/terminal.h"
#include "drivers/8250uart.h"
#include "drivers/8254pit.h"
#include "drivers/8259pic.h"
#include "interrupt/irq.h"

void dbg_fmt_ptr(char *buf, uintptr_t ptr) {
    int i = 0;
    for (int bit = 60; bit >= 0; bit -= 4) {
        char value = (ptr >> bit) & 0xF;
        if (value >= 0xA) {
            buf[i] = (value - 0xA + 'A');
        } else {
            buf[i] = (value + '0');
        }
        i++;
    }
    buf[i] = '\n';
}


int main(int mb, uintptr_t mb_info) {
    term_init();
    printf("Terminal Initialized\n");
    uart_init();
    printf("UART Initialized\n");
    remap_pic();
    printf("PIC remapped\n");
    unmask_irq(0);
    mask_irq(1);
    unmask_irq(2);
    mask_irq(3);
    printf("IRQ0 Unmasked\n");
    setup_interval_timer(100);
    printf("Interval Timer Initialized\n");
    enable_irqs();
    __asm__("sti");
    printf("IRQs Enabled\n");

    printf("\n\n\n");

    printf("Hello World\n");
    printf("Hello World\n");


    printf("  %%i: %i %i %i\n", 0, 1234, -1);
    printf("  %%u: %u %u %u\n", 0, 1234, -1);
    printf("  %%x: %x %x %x\n", 0, 1234, -1);

    com1.write("Hello World\r\n", 13);

    halt();
    return 0;
}

