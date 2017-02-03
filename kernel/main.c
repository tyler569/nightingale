
/* Set up environment
 * i.e. GDT, ISRs, and IRQs
 * to pass execution to the arch-independant kernel
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/cpu.h>
#include <kernel/tty.h>

#include "multiboot.h"

/* kernel entry point - called in boot.S after paging set up */
void kmain(multiboot_info_t *mbdata) {

    terminal_initialize();
    gdt_install();
    idt_install(); //TODO: break back out isr_install()
    irq_install();
    __asm__ ( "sti" ); //TODO: do something more reasonable with this
    timer_install();
    irq_install_handler(1, keyboard_echo_handler); //TODO: keyboard_initialize()

    printf("CPU Ready\n"); 

    if (mbdata->flags & MULTIBOOT_INFO_MEMORY) {
        printf("basic data available\n");
    } else {
        printf("no basic data available\n");
    }
    if (mbdata->flags & MULTIBOOT_INFO_MEM_MAP) {
        printf("full data available\n");
    } else {
        printf("no full data available\n");
    }

    multiboot_memory_map_t *mbmap = (multiboot_memory_map_t *)mbdata->mmap_addr;
    size_t mbmap_max = mbdata->mmap_length / sizeof(multiboot_memory_map_t);

    for (size_t i = 0; i < mbmap_max; i++) {
        printf("Addr: %i, Len: %i\n", (mbmap+i)->addr, (mbmap+i)->len);
    }

    printf("Project Nightingale\n");

    abort();

    __builtin_unreachable();

}
