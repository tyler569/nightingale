
/* Set up environment
 * i.e. GDT, ISRs, and IRQs
 * to pass execution to the arch-independant kernel
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <kernel/cpu.h>
#include <kernel/tty.h>

void afterboot_init() {
    terminal_initialize();
    gdt_install();
    idt_install();
    irq_install();
    __asm__ ( "sti" );
    timer_install();
    irq_install_handler(1, keyboard_echo_handler);

    printf("CPU Ready\n");

}

