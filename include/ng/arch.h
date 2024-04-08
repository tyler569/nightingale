#pragma once

#include <sys/cdefs.h>

BEGIN_DECLS

struct interrupt_frame;

void arch_init(void);
void arch_ap_setup(int cpu);
void arch_ap_init(void);

END_DECLS

