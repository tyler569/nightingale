
#pragma once 

#include <stdint.h>

typedef uint16_t port;

uint8_t inb(port p);
void outb(port p, uint8_t v);

uint16_t inw(port p);
void outw(port p, uint16_t v);

uint32_t ind(port p);
void outd(port p, uint32_t v);

