
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <multiboot2.h>

#include "cpu/halt.h"
#include "term/terminal.h"
#include "term/print.h"
#include "cpu/uart.h"
#include "cpu/pit.h"
#include "cpu/pic.h"
#include "cpu/irq.h"
#include "memory/allocator.h"


int kernel_main(int mb, uintptr_t mb_info) {
    term_vga_init();
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
    printf("IRQs Enabled\n");

    heap_init();

    void *foo0 = malloc(16);
    printf("%x\n", foo0);
    void *foo1 = malloc(16);
    printf("%x\n", foo1);
    void *foo2 = malloc(16);
    printf("%x\n", foo2);
    void *foo3 = malloc(16);
    printf("%x\n", foo3);
    free(foo2);
    void *foo4 = malloc(16);
    printf("%x\n", foo4);

    
//    halt();
    volatile int x = 1;
    volatile int y = 0;
    // You really have to fight clang to get it to emit an idiv
    return x / y;
}

