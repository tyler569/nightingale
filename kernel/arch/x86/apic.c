
#include <basic.h>
#include <vmm.h> // this has to be after memory init
#include "cpu.h"
#include "apic.h"

// TEMP: different addresses per-CPU?
static usize lapic_addr;
static usize ioapic_addr;

void enable_apic(usize addr) {
    lapic_addr = addr;
    wrmsr(IA32_APIC_BASE, lapic_addr | APIC_ENABLE);

    vmm_map(lapic_addr, lapic_addr); // move later so we're not in the way

    volatile uint32_t *lapic_spiv = (volatile uint32_t *)(lapic_addr + 0xF0);
    *lapic_spiv |= 0x100; // enable bit in spurrious interrupt vector
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
    // Read value
    return *(volatile uint32_t *)(ioapic_addr + 0x10);
}

void ioapic_enable(usize addr) {

}


void lapic_timer_enable(int per_second) {
}

