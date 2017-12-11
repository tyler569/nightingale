
#include <string.h>

#include <basic.h>
#include <multiboot2.h>

#include "panic.h"
#include "cpu/halt.h"
#include "term/terminal.h"
#include "term/print.h"
#include "cpu/uart.h"
#include "cpu/pit.h"
#include "cpu/pic.h"
#include "cpu/irq.h"
#include "llio/portio.h"
#include "memory/allocator.h"
#include "memory/paging.h"


void kernel_main(usize mb_info, u64 mb_magic) {
    //term_vga_init();
    term_vga.color(COLOR_WHITE, COLOR_BLUE);
    term_vga.clear();
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

    printf("Multiboot magic: %p\n", mb_magic);
    printf("Multiboot info*: %p\n", mb_info);

    if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        panic("Hair on fire - this bootloader isn't multiboot2\n");
    }

    { // Allocation 
        u8 *alloc_test0 = malloc(16);

        printf("\nalloc_test0 = %x\n", alloc_test0);

        for (i32 i=0; i<16; i++) {
            alloc_test0[i] = i;
        }

        debug_print_mem(16, alloc_test0-4);

        free(alloc_test0);
    }

    { // Page resolution and mapping
        usize resolved1 = resolve_virtual_to_physical(0x201888);
        printf("resolved vma:%p to pma:%p\n", 0x201888, resolved1);
        map_virtual_to_physical(0x201000, 0x10000);
        usize resolved2 = resolve_virtual_to_physical(0x201888);
        printf("resolved vma:%p to pma:%p\n", 0x201888, resolved2);
    }

    { // u128 test
        printf("\n\n");
        u128 x = 0;
        x -= 1;
        debug_dump(&x); // I can't print this, but I can prove it works this way
    }
    
    { // Testing length of kernel
        extern usize _kernel_end;
        usize len = (usize)&_kernel_end - 0x100000;
        printf("Kernel is %i kilobytes long\n", len / 1024);
        printf("Kernel is %x bytes long\n", len);
    }

    { // Exit / fail test
        //panic("kernel_main tried to return!\n");
        
        printf("\n");

        volatile i32 x = 1;
        volatile i32 y = 0;
        volatile i32 z = x / y;
    }
}

