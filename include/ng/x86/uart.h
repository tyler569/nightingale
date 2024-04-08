#pragma once

#include "cpu.h"
#include "sys/cdefs.h"
#include <ng/serial.h>

BEGIN_DECLS

extern struct serial_device *x86_com[2];

void x86_uart_init(void);

END_DECLS

