#include <assert.h>
#include <ng/common.h>
#include <ng/limine.h>
#include <ng/x86/acpi.h>
#include <ng/x86/apic.h>
#include <stdatomic.h>
#include <stdio.h>

constexpr size_t madt_entries_offset = offsetof(acpi_madt_t, entries);

static virt_addr_t ioapic_mapped_address = 0;

static void ioapic_mmio_w(uint32_t reg, uint32_t value)
{
    assert(!(reg & ~0xff));
    *(volatile uint32_t *)ioapic_mapped_address = reg;
    *(volatile uint32_t *)(ioapic_mapped_address + 16) = value;
}

[[maybe_unused]] static uint32_t ioapic_mmio_r(uint32_t reg)
{
    assert(!(reg & ~0xff));
    *(volatile uint32_t *)ioapic_mapped_address = reg;
    return *(volatile uint32_t *)(ioapic_mapped_address + 16);
}

union relocation_entry {
    struct {
        uint64_t vector : 8;
        uint64_t delivery_mode : 3;
        uint64_t destination_mode : 1;
        uint64_t delivery_status : 1;
        uint64_t pin_polarity : 1;
        uint64_t remote_irr : 1;
        uint64_t trigger_mode : 1;
        uint64_t mask : 1;
        uint64_t reserved : 39;
        uint64_t destination : 8;
    };
    struct {
        uint32_t low_word;
        uint32_t high_word;
    };
};

static void write_relocation(uint32_t irq, relocation_entry entry)
{
    uint32_t reg_base = 0x10 + irq * 2;

    ioapic_mmio_w(reg_base, entry.low_word);
    ioapic_mmio_w(reg_base + 1, entry.high_word);
}

void ioapic_init(acpi_madt_t *madt)
{
    // get ioapic address
    uint64_t ioapic_address = 0;
    for (size_t i = madt_entries_offset; i < madt->header.length;) {
        auto *entry = static_cast<acpi_madt_entry_t *>(PTR_ADD(madt, i));

        if (entry->type == MADT_ENTRY_IOAPIC) {
            ioapic_address = entry->ioapic.address;
            break;
        }

        i += entry->length;
    }
    printf("x86/ioapic: ioapic address is %#lx\n", ioapic_address);
    ioapic_mapped_address = limine_hhdm() | ioapic_address;

    for (int i = 1; i < 24; i++) {
        relocation_entry relo = {
            .vector = static_cast<uint64_t>(i + 0x20),
            .destination = 0,
        };

        write_relocation(i, relo);
    }

    for (size_t i = madt_entries_offset; i < madt->header.length;) {
        auto *entry = static_cast<acpi_madt_entry_t *>(PTR_ADD(madt, i));

        if (entry->type == MADT_ENTRY_ISO) {
            relocation_entry relo = {
                .vector = static_cast<uint64_t>(entry->iso.irq_source + 0x20),
                .destination = 0,
            };

            printf("x86/ioapic: routing irq %i to %i\n", entry->iso.irq_source,
                entry->iso.global_system_interrupt);

            write_relocation(entry->iso.global_system_interrupt, relo);
        }

        i += entry->length;
    }

    relocation_entry relo = {
        .vector = 0x20,
        .mask = 1,
        .destination = 0,
    };

    write_relocation(2, relo);
}
