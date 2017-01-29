
/* Set up environment
 * i.e. GDT, ISRs, and IRQs
 * to pass execution to the arch-independant kernel
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <kernel/cpu.h>
#include <kernel/tty.h>
#include <kernel/mp.h>

void initialize() {
    terminal_initialize();
    gdt_install();
    idt_install();
    irq_install();
    __asm__ ( "sti" );
    timer_install();
    irq_install_handler(1, keyboard_echo_handler);
    printf("CPU Ready\n"); /* - initializing mp\n");
    *(int *)0x1000000 = 0xAAAAAAAA;
    *(int *)0x1000004 = 0xBBBBBBBB;
    *(int *)0x1000008 = 0xCCCCCCCC;
    *(int *)0x100000c = 0xDDDDDDDD;
    *(int *)0x1000010 = 0xEEEEEEEE;
    *(int *)0x1000014 = 0xFFFFFFFF;
    mp_initialize();*/

}

