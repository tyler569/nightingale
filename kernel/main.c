
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <kernel/cpu.h>
#include <kernel/memory.h>
#include <kernel/tty.h>
#include <kernel/printk.h>

#include "multiboot.h"

/*
 * Ideas for future plans
 *
 * - break global things out into a klibc
 *    - printf
 *    - stdlib (io/etc)
 *    - malloc (against vmm/pmm)
 */


void kmain(multiboot_info_t *mbdata) {

    terminal_initialize();
    klog("Terminal Initialized");
    gdt_install();
    klog("GDT Installed");
    //TODO: break back out isr_install()
    idt_install();
    klog("Interrupt Table Installed");
    irq_install();
    timer_install();
    //TODO: do something sensible here
    __asm__ ( "sti" ); 
    klog("Interrupt Requests Initialized");
    __asm__ ( "fninit" );
    klog("FPU Enabled");
    klog("CPU Ready");

    /*
     * pmm_init(mbdata); <- physical memory manager - allocates physical pages
     * vmm_init();       <- virtual memory manager - manages virtual pages
     * kmalloc_init();   <- malloc - on demand memory
     */

    
    __builtin_cpu_init();
    if (__builtin_cpu_supports("sse")) {
        printk("CPU is sse\n");
    }



    double foo = 1.0;
    double bar = foo * 2.5;
    printk("%llx\n", *(long long *)&bar);



    klog("Project Nightingale");

    abort();

    __builtin_unreachable();

}

