
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "multiboot2.h"
#include "halt.h"
#include "video/terminal.h"
#include "drivers/uart.h"
#include "drivers/pit.h"
#include "drivers/pic.h"
#include "interrupt/irq.h"


int kernel_main(int mb, uintptr_t mb_info) {
    term_init();
    printf("Terminal Initialized\n");
    uart_init(COM1);
    printf("UART Initialized\n");
    remap_pic();
    printf("PIC remapped\n");
    setup_interval_timer(100);
    printf("Interval Timer Initialized\n");
    uart_enable_interrupt(COM1);
    printf("Serial Interrupts Initialized\n");
    enable_irqs();
    printf("IRQs Enabled\n++\n++\n");

    uart_write(COM1, "Hello World\r\n", 13);
    printf("Hello World\n");

    
//    halt();
    volatile int x = 1;
    volatile int y = 0;
    // You really have to fight clang to get it to emit an idiv
    return x / y;
}

