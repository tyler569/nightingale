
#pragma once 
#ifndef NIGHTINGALE_PORTIO_H
#define NIGHTINGALE_PORTIO_H

#include <basic.h>

typedef u16 port;

u8 inb(port p);
void outb(port p, u8 v);

u16 inw(port p);
void outw(port p, u16 v);

u32 ind(port p);
void outd(port p, u32 v);

#endif
