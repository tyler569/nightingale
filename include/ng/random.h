#pragma once
#include "sys/cdefs.h"

BEGIN_DECLS

void add_to_random(const char *buffer, size_t len);
void random_dance(void);
size_t get_random(char *buffer, size_t len);

END_DECLS