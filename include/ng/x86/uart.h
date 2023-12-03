#pragma once
#ifndef _X86_UART_H_
#define _X86_UART_H_

#include "cpu.h"
#include "sys/cdefs.h"
#include <ng/serial.h>

BEGIN_DECLS

extern struct serial_device *x86_com[2];

void x86_uart_init(void);

END_DECLS

#endif // _X86_UART_H_
