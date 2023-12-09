#pragma once
#ifndef NG_IRQ_H
#define NG_IRQ_H

#include <ng/cpu.h>
#include <sys/cdefs.h>

BEGIN_DECLS

void irq_install(int irq, void (*fn)(interrupt_frame *, void *), void *);
void irq_handler(interrupt_frame *);

END_DECLS

#endif // NG_IRQ_H
