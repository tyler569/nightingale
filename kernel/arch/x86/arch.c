#include <ng/arch.h>
#include <x86/acpi.h>
#include <x86/apic.h>
#include <x86/cpu.h>
#include <x86/pic.h>
#include "../../limine.h"

void arch_init(void)
{
    acpi_rsdp_t *rsdp = limine_rsdp();
    acpi_rsdt_t *rsdt = acpi_rsdt(rsdp);
    void *madt = acpi_get_table("MADT");

    pic_init();
    // pic_irq_unmask(0); // Timer
    // pic_irq_unmask(4); // Serial
    // pic_irq_unmask(3); // Serial COM2

    // ioapic_init(); TODO
    lapic_init();

    if (supports_feature(_X86_FSGSBASE)) {
        enable_bits_cr4(1 << 16); // enable fsgsbase
    }
}