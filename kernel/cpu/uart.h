
#pragma once
#ifndef NIGHTINGALE_UART_H
#define NIGHTINGALE_UART_H

#include <basic.h>
#include <llio/portio.h>

#define COM1 (port)0x3f8

void uart_init(port p);

void uart_write(port p, const char *buf, usize len);
char uart_read_byte(port com);
void uart_enable_interrupt(port com);
void uart_disable_interrupt(port com);

#endif
