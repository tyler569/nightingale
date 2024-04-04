#pragma once
#ifndef NG_ARCH_H
#define NG_ARCH_H

#include "sys/cdefs.h"

BEGIN_DECLS

void arch_init(void);
void arch_ap_setup(int cpu);
void arch_ap_init(void);

END_DECLS

#endif // NG_ARCH_H
