
#pragma once
#ifndef NIGHTINGALE_X86_APIC_H
#define NIGHTINGALE_X86_APIC_H

#include <basic.h>

#define MSR_IA32_APIC_BASE 0x1B

#include <basic.h>
 
// Confirm this through ACPI, remap as needed:
#define X86_LAPIC_DEFAULT_BASE 0xFEE00000
 
#define X86_MSR_IA32_APIC_BASE 0x1B
#define X86_MSR_IA32_APIC_BASE_APIC_ENABLE (1 << 11)
#define X86_MSR_IA32_APIC_BASE_CPU_IS_BSP  (1 << 8)
 

// Registers:
#define X86_LAPIC_REG_ID                    0x20
#define X86_LAPIC_REG_VERSION               0x30
#define X86_LAPIC_REG_TASK_PRIORITY         0x80
#define X86_LAPIC_REG_ARB_PRIORITY          0x90
#define X86_LAPIC_REG_PROCESSOR_PRIORITY    0xA0
#define X86_LAPIC_REG_EOI                   0xB0
#define X86_LAPIC_REG_REMOTE_READ           0xC0
#define X86_LAPIC_REG_LOGICAL_DESTINATION   0xD0
#define X86_LAPIC_REG_DESTINATION_FORMAT    0xE0
#define X86_LAPIC_REG_SPURIOUS_INTERRUPT    0xF0
// TMR - 8 dwords
// IRR - 8 dwords
#define X86_LAPIC_REG_ERROR_STATUS         0x280
#define X86_LAPIC_REG_LVT_CMCI             0x2F0
// ICR - 2 dwords
#define X86_LAPIC_REG_LVT_TIMER            0x320
#define X86_LAPIC_REG_LVT_THERMAL          0x330
#define X86_LAPIC_REG_LVT_PERFMON          0x340
#define X86_LAPIC_REG_LVT_LINT0            0x350
#define X86_LAPIC_REG_LVT_LINT1            0x360
#define X86_LAPIC_REG_LVT_ERROR            0x370
#define X86_LAPIC_REG_TIMER_INITIAL        0x380
#define X86_LAPIC_REG_TIMER_CURRENT        0x390
#define X86_LAPIC_REG_TIMER_DIVIDE         0x3E0


void apic_mode_enable();
void apic_mode_x2apic_enable();

void apic_stuff();

#endif
