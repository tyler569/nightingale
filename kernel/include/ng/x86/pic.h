#pragma once
#ifndef NG_X86_PIC_H
#define NG_X86_PIC_H

#include <basic.h>

void pic_send_eoi(int irq);

void pic_init();

void pic_irq_unmask(int irq);
void pic_irq_mask(int irq);

#endif // NG_X86_PIC_H
