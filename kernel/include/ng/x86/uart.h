#pragma once
#ifndef NG_X86_UART_H
#define NG_X86_UART_H

#include <basic.h>
#include <ng/x86/cpu.h>

#define COM1 (port_addr_t) 0x3f8
#define COM2 (port_addr_t) 0x2f8

void x86_uart_init(void);
void x86_uart_write(port_addr_t p, const char *buf, size_t len);
void x86_uart_write_byte(port_addr_t p, const char b);
char x86_uart_read_byte(port_addr_t com);
void x86_uart_enable_interrupt(port_addr_t com);
void x86_uart_disable_interrupt(port_addr_t com);

#endif // NG_X86_UART_H
