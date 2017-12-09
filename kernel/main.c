
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
#include "memory/paging.h"


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

    char *alloc_test0 = malloc(16);
    void *alloc_test1 = malloc(16);
    void *alloc_test2 = malloc(16);
    void *alloc_test3 = malloc(16);
    free(alloc_test2);
    void *alloc_test4 = malloc(16);

    printf("\nalloc_test0 = %x\n", alloc_test0);

    for (int i=0; i<16; i++) {
        alloc_test0[i] = i;
    }

    debug_print_mem(16, alloc_test0-4);

    printf("Dump allocations:\n");
    debug_dump(alloc_test0);

    printf("Dump code:\n");
    debug_dump(&printf);

    printf("Dump stack:\n");
    debug_dump(&mb);

    printf("%p\n", alloc_test0);

    uintptr_t page_table = resolve_virtual_to_physical(&debug_dump);
    printf("%p\n", page_table);
    debug_dump(page_table);

    
//    halt();
    volatile int x = 1;
    volatile int y = 0;
    // You really have to fight clang to get it to emit an idiv
    return x / y;
}

