
#include <ng/panic.h>
#include <ng/string.h>
#define DEBUG 1
#include <ng/debug.h>
#include <ng/vmm.h>
#include <ng/x86/acpi.h>

static acpi_rsdt *acpi_rsdt_cache;

acpi_rsdt *acpi_get_rsdt(acpi_rsdp *rsdp) {

        // TODO work with XSDT and 64bit ACPI base

        acpi_rsdt_cache = (void *)(uintptr_t)rsdp->rsdt_address;
        return (void *)(uintptr_t)rsdp->rsdt_address;
}

static const char *table_signatures[] = {
        [RSDT] = RSDT_SIGNATURE,
        [FADT] = FADT_SIGNATURE,
        [SSDT] = SSDT_SIGNATURE,
        [MADT] = MADT_SIGNATURE,
};

void *acpi_get_table(int table_id) {
        if (!(table_id >= RSDT && table_id <= MADT))
                panic("Invalid ACPI table");
        if (!acpi_rsdt_cache)
                panic("RSDT location not initialized");

        if (table_id == RSDT)
                return acpi_rsdt_cache;

        acpi_rsdt *rsdt = acpi_rsdt_cache; // brevity
        int entry_count = ((rsdt->header.length - sizeof(rsdt->header)) / 4);

        if (entry_count < 0)
                panic("ACPI RSDT header indicates negative entries");

        for (size_t i = 0; i < entry_count; i++) {

                acpi_header *h = (acpi_header *)(uintptr_t)rsdt->table_ptr[i];
                vmm_map((uintptr_t)h, (uintptr_t)h, PAGE_WRITEABLE);

                if (strncmp(h->signature, table_signatures[table_id], 4) == 0)
                        return h;
        }

        // Table not found
        return NULL;
}

void acpi_print_header(acpi_header *header) {
        printf("acpi: table '%.4s' (%.6s)\n", &header->signature,
               &header->creator_id);
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

        if (table_type == -1)
                panic("Invalid table to print");

        acpi_print_header(table);

        switch (table_type) {
        case RSDT: {
                // acpi_rsdt *rsdt = (void *)table;
                break;
        }
        case MADT: {
                acpi_madt *madt = (void *)table;
                printf("  madt: LAPIC address: %#x\n", madt->lapic_address);

                printf("  madt: APIC entries:\n");
                size_t current = sizeof(acpi_header) + 8;
                while (current < madt->header.length) {
                        acpi_madt_entry *entry =
                            (void *)((char *)madt + current);
                        switch (entry->type) {
                        case MADT_ENTRY_LAPIC: {
                                printf("  madt: LAPIC %hhu/processor %hhu\n",
                                       entry->lapic.id,
                                       entry->lapic.processor_id);
                                break;
                        }
                        case MADT_ENTRY_IOAPIC: {
                                printf("  madt: IOAPIC %hhu @%#x base: %u\n",
                                       entry->ioapic.id, entry->ioapic.address,
                                       entry->ioapic.interrupt_base);
                                break;
                        }
                        case MADT_ENTRY_ISO: {
                                printf("  madt: ISO: irq %u->%u\n",
                                       entry->iso.irq_source,
                                       entry->iso.global_system_interrupt);
                                break;
                        }
                        case MADT_ENTRY_NMI: {
                                printf("  madt: NMI LINT#%u\n",
                                       entry->nmi.LINT_number);
                                break;
                        }
                        case MADT_ENTRY_LAPIC_ADDRESS: {
                                printf("  madt: LAPIC Address override: %lp\n",
                                       entry->lapic_address.address);
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
