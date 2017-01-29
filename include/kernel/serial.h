
#pragma once

#include <stdint.h>

void serial_initialize();

int serial_received();
uint8_t serial_read();
int serial_transmit_empty();
void serial_write(uint8_t a);

