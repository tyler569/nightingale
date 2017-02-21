
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <multiboot.h>
#include <kernel/cpu.h>
#include <kernel/mem/pmm.h>
#include <kernel/mem/vmm.h>
#include <kernel/heap.h>
#include <kernel/tty.h>
#include <kernel/printk.h>

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
    idt_install();
    klog("Interrupt Table Installed");
    irq_install();
    timer_install();
    sti();
    klog("Interrupt Requests Initialized");
    __asm__ ( "fninit" );
    klog("FPU Enabled");
    klog("CPU Ready");

    pmm_init(mbdata);// <- physical memory manager - allocates physical pages
    // vmm_init();   // <- virtual memory manager - manages virtual pages
    heap_init();


    printk("%08x\n", (unsigned int)*(short *)0x40E);

    for (int i=0x9FC00; i<0x9FFFF; i += 16) {
        if (memcmp((void *)i, (void *)"RSD PTR ", 8) == 0) {
            printk("%08x\n", i);
        }
    }


    klog("Project Nightingale");

    abort();

    __builtin_unreachable();

}

