
#pragma once
#ifndef NIGHTINGALE_PIC_H
#define NIGHTINGALE_PIC_H

#include <ng/basic.h>

void pic_send_eoi(int irq);

void pic_init();

void pic_irq_unmask(int irq);
void pic_irq_mask(int irq);

#endif
