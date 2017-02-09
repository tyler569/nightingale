
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

    // paging_system_init(mbdata)

    printk("\t%x is mapped to %lx\n", vma_to_pma   , vma_to_pma(vma_to_pma));
    printk("\t%x is mapped to %lx\n", 0x1000       , vma_to_pma(0x1000));
    printk("\t%x is mapped to %lx\n", 0x90000000   , vma_to_pma(0x90000000));
    printk("\tMap 0x90000000\n");
    map_4M_page(0x90000000, 0x10000000);
    printk("\t%x is mapped to %lx\n", 0x90000000   , vma_to_pma(0x90000000));
    *(int *)(0x90000000) = 0xDEADBEEF;
    printk("\t%x\n", *(int *)(0x90000000));
    printk("\tUnmap 0x90000000\n");
    unmap_page(0x90000000);
    printk("\t%x is mapped to %lx\n", 0x90000000   , vma_to_pma(0x90000000));
    __asm__ ("invlpg (0x90000000)");
    printk("\t%x\n", *(int *)(0x90000000));

    printk("\t%lx\n", &end_kernel);
    

    klog("Project Nightingale");

    abort();

    __builtin_unreachable();

}

