
#pragma once
#ifndef NIGHTINGALE_UART_H
#define NIGHTINGALE_UART_H

#include <ng/basic.h>

void uart_init();
void uart_write(const char* buf, size_t len);
char uart_read_byte();

#endif
