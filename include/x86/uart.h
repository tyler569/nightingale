#pragma once
#ifndef _X86_UART_H_
#define _X86_UART_H_

#include <ng/serial.h>
#include <sys/cdefs.h>
#include <x86/cpu.h>

BEGIN_DECLS

extern struct serial_device *x86_com[2];

void x86_uart_init(void);

END_DECLS

#endif // _X86_UART_H_
