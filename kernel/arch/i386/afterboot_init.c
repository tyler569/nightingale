
/* Set up environment
 * i.e. GDT, ISRs, and IRQs
 * to pass execution to the arch-independant kernel
 */

#include <stdint.h>
#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/printk.h>

#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"
#include "keyboard.h"

void afterboot_init() {
    terminal_initialize();
    printk(LOG_INFO "TTY initialized");

    gdt_install();
    printk(LOG_INFO "GDT installed");

    idt_install();
    printk(LOG_INFO "IDT installed");

    irq_install();
    __asm__ ( "sti" );
    printk(LOG_INFO "IRQs initialized");

    timer_install();
    printk(LOG_INFO "Timer initialized");

    //keyboard_initialized
    irq_install_handler(1, keyboard_echo_handler);
    printk(LOG_INFO "Keyboard ready");

}

