
#pragma once
#ifndef NIGHTINGALE_ARCH_X86_UART_H
#define NIGHTINGALE_ARCH_X86_UART_H

#include <ng/basic.h>
#include "portio.h"

#define COM1 (port)0x3f8

void x86_uart_init(port p);

void x86_uart_irq_handler();

void x86_uart_write(port p, const char *buf, size_t len);
void x86_uart_write_byte(port p, const char b);
char x86_uart_read_byte(port com);
void x86_uart_enable_interrupt(port com);
void x86_uart_disable_interrupt(port com);

#endif