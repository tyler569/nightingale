
#pragma once
#ifndef NIGHTINGALE_X86_APIC_H
#define NIGHTINGALE_X86_APIC_H

#include <basic.h>

#define MSR_IA32_APIC_BASE 0x1B

void apic_mode_enable();
void apic_mode_x2apic_enable();

void apic_stuff();

#endif
