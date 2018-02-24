
#include <basic.h>
#include <arch/x86/cpu.h>
#include <arch/x86/apic.h>

void apic_enable(usize apic_addr) {
    wrmsr(MSR_IA32_APIC_BASE, apic_addr | MSR_IA32_APIC_BASE_ENABLE);

    // Need paging first
    // u32 *lapic_timer = apic_addr + 0x320;
    // *lapic_timer = 0;
}

