
#include <basic.h>
#include <ng/vmm.h>
#include <x86/cpu.h>

#define LAPIC_ID 0x020
#define LAPIC_VERSION 0x030
#define LAPIC_TPR 0x080     // Task Priority Register
#define LAPIC_APR 0x090     // Arbitration Priority Register
#define LAPIC_PPR 0x0A0     // Processor Priority Register
#define LAPIC_EOI 0x0B0     // End Of Interrupt Register
#define LAPIC_RRD 0x0C0     // Remote Read Register
#define LAPIC_LDR 0x0D0     // Logical Destination Register
#define LAPIC_DFR 0x0E0     // Destination Format Register
#define LAPIC_SVR 0x0F0     // Spurious Interrupt Vector Register
#define LAPIC_ISR 0x100 // -> 0x170  In-Service Register
#define LAPIC_TMR 0x180 // -> 0x1F0  Trigger Mode Register
#define LAPIC_IRR 0x200 // -> 0x270  Interrupt Request Register
#define LAPIC_ESR 0x280     // Error Status Register
#define LAPIC_LVT_CMCI 0x2F0 // LVT Corrected Machine Check Interrupt Register
// intentionally 2 -> 300 -- writes go to 310 first
#define LAPIC_ICR2 0x300    // Interrupt Command Register
#define LAPIC_ICR1 0x310    // Interrupt Command Register
#define LAPIC_LVT_TIMER 0x320   // LVT Timer Register
#define LAPIC_LVT_THERMAL 0x330 // LVT Thermal Sensor Register
#define LAPIC_LVT_PMC 0x340     // LVT Performance Monitoring Counter Register
#define LAPIC_LVT_LINT0 0x350   // LVT LINT0 Register
#define LAPIC_LVT_LINT1 0x360   // LVT LINT1 Register
#define LAPIC_LVT_ERROR 0x370   // LVT Error Register
#define LAPIC_TIMER_ICR 0x380   // LVT Timer Initial Count Register
#define LAPIC_TIMER_CCR 0x390   // LVT Timer Current Count Register
#define LAPIC_TIMER_DCR 0x3E0   // LVT Timer Divide Configuration Register

void lapic_init();
void lapic_eoi(int interrupt_number);
void lapic_send_init(int destination_processor);
void lapic_send_ipi(int type, int vector, int destination_processor);
