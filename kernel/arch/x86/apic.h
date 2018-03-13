
#pragma once
#ifndef NIGHTINGALE_APIC_H
#define NIGHTINGALE_APIC_H

#include <basic.h>
 
// Confirm this through ACPI, remap as needed:

#define LAPIC_DEFAULT_BASE 0xFEE00000
 
#define IA32_APIC_BASE 0x1B
#define APIC_ENABLE (1 << 11)
// #define APIC_BSP    (1 << 8)
 

// Registers:
#define LAPIC_REG_ID                    0x20
#define LAPIC_REG_VERSION               0x30
#define LAPIC_REG_TASK_PRIORITY         0x80
#define LAPIC_REG_ARB_PRIORITY          0x90
#define LAPIC_REG_PROCESSOR_PRIORITY    0xA0
#define LAPIC_REG_EOI                   0xB0
#define LAPIC_REG_REMOTE_READ           0xC0
#define LAPIC_REG_LOGICAL_DESTINATION   0xD0
#define LAPIC_REG_DESTINATION_FORMAT    0xE0
#define LAPIC_REG_SPURIOUS_INTERRUPT    0xF0
// TMR - 8 dwords
// IRR - 8 dwords
#define LAPIC_REG_ERROR_STATUS         0x280
#define LAPIC_REG_LVT_CMCI             0x2F0
// ICR - 2 dwords
#define LAPIC_REG_LVT_TIMER            0x320
#define LAPIC_REG_LVT_THERMAL          0x330
#define LAPIC_REG_LVT_PERFMON          0x340
#define LAPIC_REG_LVT_LINT0            0x350
#define LAPIC_REG_LVT_LINT1            0x360
#define LAPIC_REG_LVT_ERROR            0x370
#define LAPIC_REG_TIMER_INITIAL        0x380
#define LAPIC_REG_TIMER_CURRENT        0x390
#define LAPIC_REG_TIMER_DIVIDE         0x3E0



int enable_apic(uintptr_t addr);

void apic_mode_enable();
void apic_mode_x2apic_enable();

void apic_stuff();

#endif
