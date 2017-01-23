
/* Set up environment
 * i.e. GDT, ISRs, and IRQs
 * to pass execution to the arch-independant kernel
 */

#include <stdint.h>
#include <stdio.h>
#include <kernel/tty.h>

#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"
#include "keyboard.h"

void afterboot_init() {
    terminal_initialize();
    printf("TTY initialised\n"); //v

    gdt_install();
    printf("GDT installed\n"); //replace with some kind of klog

    idt_install();
    printf("IDT installed\n"); //^

    irq_install();
    __asm__ ( "sti" );
    printf("IRQs initialised\n"); //^

    timer_install();

    irq_install_handler(1, keyboard_echo_handler);

}

