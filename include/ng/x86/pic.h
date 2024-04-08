#pragma once

#include "sys/cdefs.h"

enum pic_interrupt {
	IRQ_TIMER = 0,
	IRQ_KEYBOARD = 1,
	IRQ_SERIAL2 = 3,
	IRQ_SERIAL1 = 4,
	IRQ_LPT2 = 5,
	IRQ_FLOPPY = 6,
	IRQ_LPT1 = 7,
	IRQ_RTC = 8,
	IRQ_MOUSE = 12,
	IRQ_FPU = 13,
	IRQ_ATA1 = 14,
	IRQ_ATA2 = 15,
};

BEGIN_DECLS

void pic_send_eoi(int irq);
void pic_init();
void pic_irq_unmask(int irq);
void pic_irq_mask(int irq);

END_DECLS

