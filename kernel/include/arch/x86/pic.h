
#pragma once
#ifndef NIGHTINGALE_PIC_H
#define NIGHTINGALE_PIC_H

#include <basic.h>

void pic_send_eoi(i32 irq);

void pic_init();

void pic_irq_unmask(i32 irq);
void pic_irq_mask(i32 irq);

#endif
