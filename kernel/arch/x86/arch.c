#include <assert.h>
#include <ng/arch.h>
#include <ng/thread.h>
#include <x86/acpi.h>
#include <x86/apic.h>
#include <x86/cpu.h>
#include <x86/pic.h>
#include "../../limine.h"

void arch_init(void)
{
    uint64_t tmp;
    __asm__ volatile("mov %%cr3, %0" : "=a"(tmp));
    running_process->vm_root = tmp & 0x00FFFFFFFFFFF000;

    acpi_rsdp_t *rsdp = limine_rsdp();
    acpi_init(rsdp);
    void *madt = acpi_get_table("APIC");
    assert(madt);

    pic_init();
    // pic_irq_unmask(0); // Timer
    // pic_irq_unmask(4); // Serial
    // pic_irq_unmask(3); // Serial COM2

    ioapic_init(madt);
    lapic_init();

    if (supports_feature(_X86_FSGSBASE)) {
        enable_bits_cr4(1 << 16); // enable fsgsbase
    }
}