
#pragma once
#ifndef NIGHTINGALE_PIC_H
#define NIGHTINGALE_PIC_H

#include <basic.h>

void send_end_of_interrupt(i32 irq);

void remap_pic();

void unmask_irq(i32 irq);
void mask_irq(i32 irq);

#endif
