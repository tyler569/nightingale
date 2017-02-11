
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

    printk("\t%lx\n", &end_kernel);

    /* 
    map_4M_page(KERNEL_PD, 0x10000000, 0x10000000);
    *(int *)0x10000000 = 0x1234CAFE;
    printk("%i\n", *(int *)0x10000000); 
    print_memory_map();
    unmap_page(KERNEL_PD, 0x10000000);
    print_memory_map();
    */

    int *foo = kmalloc(sizeof(int));
    *foo = 123456; 
    printk("%x : %i\n", foo, *foo);
  
    int *bar = kmalloc(sizeof(int));
    *bar = 123456; 
    printk("%x : %i\n", bar, *bar);
    

    klog("Project Nightingale");

    abort();

    __builtin_unreachable();

}

