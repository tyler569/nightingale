#pragma once

#include <stddef.h>
#include <stdint.h>

void hexdump_addr(const void *data, size_t len, uintptr_t base_address);
void hexdump(const void *data, size_t len);
