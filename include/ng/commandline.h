#pragma once

#include "sys/cdefs.h"

BEGIN_DECLS

void init_command_line(void);
const char *get_kernel_argument(const char *key);

END_DECLS
