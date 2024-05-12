#pragma once

#include "cpu.h"
#include "sys/cdefs.h"
#include <ng/serial.h>

BEGIN_DECLS

extern struct serial_device serial_devices[];

void x86_uart_init();

END_DECLS
