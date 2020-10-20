#pragma once
#ifndef NG_X86_PORTIO_H
#define NG_X86_PORTIO_H

#include <basic.h>

typedef uint16_t port;

unsigned char inb(port p);
void outb(port p, unsigned char v);

uint16_t inw(port p);
void outw(port p, uint16_t v);

uint32_t ind(port p);
void outd(port p, uint32_t v);

#endif // NG_X86_PORTIO_H
