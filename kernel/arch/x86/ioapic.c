#include <assert.h>
#include <ng/common.h>
#include <ng/x86/acpi.h>
#include <stdatomic.h>
#include <stdio.h>

static const uint32_t ioapic_linear_address = 0xFEC00000;
static const uintptr_t ioapic_mapped_address = 0xFFFF8000FEC00000;

static void ioapic_mmio_w(int reg, uint32_t value)
{
    assert(!(reg & ~0xff));
    *(volatile uint32_t *)(ioapic_mapped_address + 0) = reg;
    *(volatile uint32_t *)(ioapic_mapped_address + 16) = value;
}

__MAYBE_UNUSED
static uint32_t ioapic_mmio_r(int reg)
{
    assert(!(reg & ~0xff));
    *(volatile uint32_t *)(ioapic_mapped_address + 0) = reg;
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

static void write_relocation(int irq, union relocation_entry entry)
{
    int reg_base = 0x10 + irq * 2;

    ioapic_mmio_w(reg_base, entry.low_word);
    ioapic_mmio_w(reg_base + 1, entry.high_word);
}

void ioapic_init(acpi_madt_t *madt)
{
    for (int i = 1; i < 24; i++) {
        union relocation_entry relo = {
            .vector = i + 0x20,
            .destination = 0,
        };

        write_relocation(i, relo);
    }

    unsigned length = offsetof(acpi_madt_t, entries);
    while (length < madt->header.length) {
        acpi_madt_entry_t *entry = PTR_ADD(madt, length);

        if (entry->type == MADT_ENTRY_ISO) {
            union relocation_entry relo = {
                .vector = entry->iso.irq_source + 0x20,
                .pin_polarity = !!(entry->iso.flags & 2),
                .trigger_mode = !!(entry->iso.flags & 8),
                .destination = 0,
            };

            write_relocation(entry->iso.global_system_interrupt, relo);
        }

        length += entry->length;
    }

    union relocation_entry relo = {
        .vector = 0x20,
        .mask = 1,
        .destination = 0,
    };

    write_relocation(2, relo);
}
