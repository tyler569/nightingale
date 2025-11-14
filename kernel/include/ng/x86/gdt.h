#pragma once

#include <sys/cdefs.h>

BEGIN_DECLS

void gdt_cpu_setup(int cpu);
void gdt_cpu_load();
void set_kernel_stack(void *sp);

END_DECLS
