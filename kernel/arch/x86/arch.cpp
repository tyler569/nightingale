#include <assert.h>
#include <ng/arch.h>
#include <ng/limine.h>
#include <ng/thread.h>
#include <ng/x86/acpi.h>
#include <ng/x86/apic.h>
#include <ng/x86/cpu.h>
#include <ng/x86/gdt.h>
#include <ng/x86/pic.h>

extern bool have_fsgsbase;

void arch_init(void)
{
    gdt_cpu_setup();

    uint64_t tmp;
    __asm__ volatile("mov %%cr3, %0" : "=a"(tmp));
    set_running_pt_root(tmp & 0x00FFFFFFFFFFF000);

    auto *rsdp = static_cast<acpi_rsdp *>(limine_rsdp());
    acpi_init(rsdp);
    void *madt = acpi_get_table("APIC");
    assert(madt);
    pic_init();
    ioapic_init(static_cast<acpi_madt_t *>(madt));
    lapic_init();

    if (supports_feature(_X86_FSGSBASE)) {
        enable_bits_cr4(1 << 16); // enable fsgsbase
        have_fsgsbase = true;
    }
}

void arch_ap_init(void)
{
    gdt_cpu_setup();
    lapic_init();
    if (supports_feature(_X86_FSGSBASE)) {
        enable_bits_cr4(1 << 16); // enable fsgsbase
    }
}