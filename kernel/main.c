
/* 
 * Set up environment
 * i.e. GDT, ISRs, and IRQs
 * to pass execution to the arch-independant kernel
 */

#include <stdint.h>
#include <stddef.h>
//#include <stdio.h>
#include <stdlib.h>

#include <kernel/cpu.h>
#include <kernel/kmem.h>
#include <kernel/tty.h>
#include <kernel/printk.h>

#include "multiboot.h"

extern int end_kernel;

/* kernel entry point - called in boot.S after paging set up */
void kmain(multiboot_info_t *mbdata) {

    terminal_initialize();
    klog("Terminal Initlized");
    gdt_install();
    klog("GDT Installed");
    idt_install(); //TODO: break back out isr_install()
    klog("Interrupt Table Installed");
    irq_install();
    timer_install();
    __asm__ ( "sti" ); //TODO: do something more reasonable with this
    klog("Interrupt Requests Initlized");
    // irq_install_handler(1, keyboard_echo_handler); //TODO: keyboard_initialize()
    klog("CPU Ready");

    memory_init(mbdata);

    printk("\tHeap starts at: %p\n", (void *)&end_kernel);

    while (true) {
        int *foo = kmalloc(0x1000000);
        if (foo == NULL) {
            break;
        }
        *foo = 123456; 
        printk("\tallocate 16MiB @ %p (%p)\n", 
                (void *)foo, (void *)vma_to_pma(KERNEL_PD, (uintptr_t)foo));
    }
    printk("OOM\n");

    klog("Project Nightingale");

    abort();

    __builtin_unreachable();

}

