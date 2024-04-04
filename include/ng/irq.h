#pragma once
#ifndef NG_IRQ_H
#define NG_IRQ_H

#include <list.h>
#include <ng/cpu.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct irq_handler {
	list_n node;
	void (*handler_func)(interrupt_frame *, void *);
	void *impl;
};

void irq_install(int irq, void (*fn)(interrupt_frame *, void *), void *);
void irq_handler(interrupt_frame *);

END_DECLS

#endif // NG_IRQ_H
