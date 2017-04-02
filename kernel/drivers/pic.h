
#pragma once

void send_end_of_interrupt(int irq);

void remap_pic();

void unmask_irq(int irq);
void mask_irq(int irq);

