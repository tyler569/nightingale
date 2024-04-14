#pragma once

#include "sys/cdefs.h"

BEGIN_DECLS

void random_write(const char *buffer, size_t len);
void random_add_boot_randomness();
size_t random_read(char *buffer, size_t len);

END_DECLS
