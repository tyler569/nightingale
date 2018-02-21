
#pragma once
#ifndef NIGHTINGALE_PIC_H
#define NIGHTINGALE_PIC_H

#include <basic.h>

void send_end_of_interrupt(i32 irq);

void remap_pic();

void pic_irc_unmask(i32 irq);
void pic_irc_mask(i32 irq);

#endif
