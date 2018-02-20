
#include <string.h>
#include <panic.h>
#define DEBUG 1
#include <debug.h>
#include <arch/x86/acpi.h>

static acpi_rsdt *acpi_rsdt_cache;

acpi_rsdt *acpi_get_rsdt(acpi_rsdp *rsdp) {
    // if (acpi_rsdt_cache)  return acpi_rsdt_cache;
    acpi_rsdt_cache = (void *)rsdp->rsdt_address;
    return (void *)rsdp->rsdt_address;
}

static const char *table_signatures[] = {
    [RSDT] = RSDT_SIGNATURE,
    [FADT] = FADT_SIGNATURE,
    [SSDT] = SSDT_SIGNATURE,
    [MADT] = MADT_SIGNATURE,
};

void *acpi_get_table(int table_id) {
    assert(table_id >= RSDT && table_id <= MADT, "Invalid table to get");
    assert(acpi_rsdt_cache, "RSDT location not initialized");

    if (table_id == RSDT)  return acpi_rsdt_cache;

    acpi_rsdt *rsdt = acpi_rsdt_cache; // brevity
    int entry_count = ((rsdt->header.length - sizeof(rsdt->header)) / 4);

    for(usize i=0; i<entry_count; i++) {
        acpi_header *h = (acpi_header *)rsdt->table_ptr[i];

        if (strncmp(h->signature, table_signatures[table_id], 4) == 0)
            return h;
    }

    // Table not found
    return NULL;
}

void acpi_print_header(acpi_header *header) {
    printf("ACPI header:\n");
    printf("    signature = %.4s\n", header->signature);
    printf("    length:   %#x\n", header->length);
    printf("    revision: %#x\n", header->revision);
    printf("    checksum: %#x\n", header->checksum);
    printf("    oem_id:       %.6s\n", header->oem_id);
    printf("    oem_table_id: %.8s\n", header->oem_table_id);
    printf("    oem_rev:     %#x\n", header->oem_revision);
    printf("    creator_id:  %#x\n", header->creator_id);
    printf("    creator_rev: %#x\n", header->creator_revision);
}

void acpi_print_table(acpi_header *table) {
    int table_type = -1;
    if (strncmp(table->signature, RSDT_SIGNATURE, 4) == 0) {
        table_type = RSDT;
    } else if (strncmp(table->signature, FADT_SIGNATURE, 4) == 0) {
        table_type = FADT;
    } else if (strncmp(table->signature, SSDT_SIGNATURE, 4) == 0) {
        table_type = SSDT;
    } else if (strncmp(table->signature, MADT_SIGNATURE, 4) == 0) {
        table_type = MADT;
    }

    assert(table_type != -1, "Invalid table to print");

    acpi_print_header(table);

    switch (table_type) {
    case RSDT: {
        printf("RSDT Things\n");
        // acpi_rsdt *rsdt = (void *)table;
        break;
    }
    case MADT: {
        acpi_madt *madt = table;
        printf("  MADT lapic address: %#x\n", madt->lapic_address);
        printf("  MADT flags:         %#x\n", madt->flags);
        printf("  MADT APIC entries:\n");
        usize current = sizeof(acpi_header) + 8;
        while (current < madt->header.length) {
            acpi_madt_entry *entry = (void *)((char *)madt + current);
            switch (entry->type) {
            case MADT_ENTRY_LAPIC: {
                acpi_madt_lapic *lapic = &entry->lapic;
                printf("    LAPIC %hhu, Processor %hhu\n", lapic->id, lapic->processor_id);
                break;
            }
            case MADT_ENTRY_IOAPIC: {
                acpi_madt_ioapic *ioapic = &entry->ioapic;
                printf("    IOAPIC %hhu\n", ioapic->id);
                printf("      Address:        %#x\n", ioapic->address);
                printf("      Interrupt Base: %#x\n", ioapic->interrupt_base);
                break;
            }
            case MADT_ENTRY_ISO: {
                acpi_madt_iso *iso = &entry->iso;
                printf("    Interrupt Source Override\n");
                printf("      Bus %u\n", iso->bus_source);
                printf("      IRQ %u\n", iso->irq_source);
                printf("      Global interrupt: %u\n", iso->global_system_interrupt);
                printf("      Flags: %#x\n", iso->flags);
                break;
            }
            case MADT_ENTRY_NMI: {
                acpi_madt_nmi *nmi = &entry->nmi;
                printf("    NMI LINT#%u\n", nmi->LINT_number);
                printf("      Processor: %u\n", nmi->processor_id);
                printf("      Flags:     %#x\n", nmi->flags);
                break;
            }
            case MADT_ENTRY_LAPIC_ADDRESS: {
                acpi_madt_lapic_address *lapic_addr = &entry->lapic_address;
                printf("    LAPIC Address override!\n");
                printf("      lapic address = %p\n", lapic_addr->address);
                break;
            }
            default:
                printf("    Unhandled MADT entry\n");
                break;
            }
            current += entry->length;
        }
        break;
    }
    default:
        break;
    }
}


