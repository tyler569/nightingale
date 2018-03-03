
#include <basic.h>
#include "cpu.h"
#include "apic.h"

// TEMP: different addresses per-CPU?
static usize lapic_addr;
static usize ioapic_addr;

void lapic_enable(usize addr) {
    lapic_addr = addr;
    wrmsr(MSR_IA32_APIC_BASE, lapic_addr | MSR_IA32_APIC_BASE_ENABLE);
}

void ioapic_write(uint8_t offset, uint32_t value) {
    // TODO: fix race condition!
    // if we get an interrupt in between these it could break shit

    // Register select
    *(volatile uint32_t *)(ioapic_addr) = offset;
    // Write value
    *(volatile uint32_t *)(ioapic_addr + 0x10) = value;
}

uint32_t ioapic_read(uint8_t offset) {
    // See write for the problem with this approach

    // Register select
    *(volatile uint32_t *)(ioapic_addr) = offset;
    // Write value
    return *(volatile uint32_t *)(ioapic_addr + 0x10);
}

void ioapic_enable(usize addr) {

}


void lapic_timer_enable(int per_second) {
}

