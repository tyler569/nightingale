#pragma once

#include <list.h>
#include <ng/cpu.h>
#include <sys/cdefs.h>

BEGIN_DECLS

struct irq_handler {
	list_node node;
	void (*handler_func)(interrupt_frame *, void *);
	void *impl;
};

void irq_install(int irq, void (*fn)(interrupt_frame *, void *), void *);
void irq_handler(interrupt_frame *);

END_DECLS

