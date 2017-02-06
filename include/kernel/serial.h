
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void serial_initialize();

uint8_t serial_read();
void serial_write_byte(uint8_t a);
int serial_write(char *data, size_t len);

