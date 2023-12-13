#pragma once
#ifndef NG_IRQ_H
#define NG_IRQ_H

#include <ng/cpu.h>
#include <nx/functional.h>

void irq_install(int irq, void (*fn)(interrupt_frame *, void *), void *);
void irq_install(int irq, nx::function<void(interrupt_frame *)> &&irq_handler);

void irq_handler(interrupt_frame *);

#endif // NG_IRQ_H
