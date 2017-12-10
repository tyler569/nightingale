
#include <string.h>

#include <basic.h>
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


void kernel_main(i32 mb, usize mb_info) {
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

    u8 *alloc_test0 = malloc(16);
    void *alloc_test1 = malloc(32);
    void *alloc_test2 = malloc(64);
    free(alloc_test2);
    void *alloc_test3 = malloc(128);
    void *alloc_test4 = malloc(16);
    free(alloc_test3);
    free(alloc_test1);
    free(alloc_test4);
    void *alloc_test5 = malloc(160);

    printf("\nalloc_test0 = %x\n", alloc_test0);

    for (i32 i=0; i<16; i++) {
        alloc_test0[i] = i;
    }

    debug_print_mem(16, alloc_test0-4);

    printf("Test memory dump of allocations:\n");
    // debug_dump(alloc_test0);

    usize resolved = resolve_virtual_to_physical((usize)alloc_test0);
    printf("resolved vma:%p to pma:%p\n", alloc_test0, resolved);

    free(alloc_test0);

    u128 x = 0;
    x -= 1;
    // debug_dump(&x);
    
    halt();
    /*
    volatile i32 x = 1;
    volatile i32 y = 0;
    volatile i32 z = x / y;
    */
}

