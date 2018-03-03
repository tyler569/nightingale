
#include <string.h>
#include <panic.h>
#define DEBUG 1
#include <debug.h>
#include "acpi.h"
#include <vmm.h>

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
    if (!(table_id >= RSDT && table_id <= MADT))  panic("Invalid ACPI table");
    if (!acpi_rsdt_cache)  panic("RSDT location not initialized");

    if (table_id == RSDT)  return acpi_rsdt_cache;

    acpi_rsdt *rsdt = acpi_rsdt_cache; // brevity
    int entry_count = ((rsdt->header.length - sizeof(rsdt->header)) / 4);

    if (entry_count < 0)  panic("ACPI RSDT header indicates negative entries");

    for (usize i=0; i<entry_count; i++) {
        // vmm_map(&rsdt->table_ptr[i], &rsdt->table_ptr[i]);
        acpi_header *h = (acpi_header *)rsdt->table_ptr[i];
        vmm_map(h, h);

        if (strncmp(h->signature, table_signatures[table_id], 4) == 0)
            return h;
    }

    // Table not found
    return NULL;
}

void acpi_print_header(acpi_header *header) {
    printf("acpi: table '%.4s' (%.6s)\n", &header->signature, &header->creator_id);
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

    if (table_type == -1)  panic("Invalid table to print");

    acpi_print_header(table);

    switch (table_type) {
    case RSDT: {
        // acpi_rsdt *rsdt = (void *)table;
        break;
    }
    case MADT: {
        acpi_madt *madt = table;
        printf("  madt: LAPIC address: %#x\n", madt->lapic_address);
        // printf("  MADT flags:         %#x\n", madt->flags);
        printf("  madt: APIC entries:\n");
        usize current = sizeof(acpi_header) + 8;
        while (current < madt->header.length) {
            acpi_madt_entry *entry = (void *)((char *)madt + current);
            switch (entry->type) {
            case MADT_ENTRY_LAPIC: {
                acpi_madt_lapic *lapic = &entry->lapic;
                printf("  madt: LAPIC %hhu/processor %hhu\n", lapic->id, lapic->processor_id);
                break;
            }
            case MADT_ENTRY_IOAPIC: {
                acpi_madt_ioapic *ioapic = &entry->ioapic;
                printf("  madt: IOAPIC %hhu @%#x base: %u\n", ioapic->id, ioapic->address, ioapic->interrupt_base);
                break;
            }
            case MADT_ENTRY_ISO: {
                acpi_madt_iso *iso = &entry->iso;
                printf("  madt: ISO: irq %u->%u\n", iso->irq_source, iso->global_system_interrupt);
                break;
            }
            case MADT_ENTRY_NMI: {
                acpi_madt_nmi *nmi = &entry->nmi;
                printf("  madt: NMI LINT#%u\n", nmi->LINT_number);
                break;
            }
            case MADT_ENTRY_LAPIC_ADDRESS: {
                acpi_madt_lapic_address *lapic_addr = &entry->lapic_address;
                printf("  madt: LAPIC Address override: %lp\n", lapic_addr->address);
                break;
            }
            default:
                // printf("    Unhandled MADT entry\n");
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


