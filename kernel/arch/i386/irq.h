
#ifndef _ARCH_I386_IRQ_H
#define _ARCH_I386_IRQ_H

#include "idt.h"

void irq_install();
void irq_install_handler(size_t irq, void (*handler)(struct regs *r));

#endif // _ARCH_I386_IRQ_H
