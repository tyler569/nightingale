
#ifndef NIGHTINGALE_ACPI_H
#define NIGHTINGALE_ACPI_H

#include <basic.h>

#define RSDP_SIGNATURE "RSD PTR "
#define RSDT_SIGNATURE "RSDT"
#define FADT_SIGNATURE "FACP"
#define SSDT_SIGNATURE "SSDT"
#define MADT_SIGNATURE "APIC"

/*
 * These are declared as an enum for convenience:
 * acpi_find_table(rsdt *, FADT);, etc.
 */
enum {
    RSDT,
    FADT,
    SSDT,
    MADT,
};

typedef struct PACKED acpi_rsdp {
    char signature[8];
    u8 checksum;
    char oem_id[6];
    u8 revision;
    u32 rsdt_address;
} acpi_rsdp;

typedef struct PACKED acpi_header {
    char signature[4];
    u32 length;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
} acpi_header;

typedef struct PACKED acpi_rsdt {
    acpi_header header;
    u32 table_ptr[0];
} acpi_rsdt;

#define MADT_LAPIC_FLAG_PROCESSOR_ENABLED 1
typedef struct PACKED acpi_madt_lapic {
    u8 processor_id;
    u8 apic_id;
    u32 flags;
} acpi_madt_lapic;

typedef struct PACKED acpi_madt_ioapic {
    u8 ioapic_id;
    u8 _reserved_0;
    u32 ioapic_address;
    u32 interrupt_base;
} acpi_madt_ioapic;

typedef struct PACKED acpi_madt_entry {
    u8 type;
    u8 length;
    union {
        acpi_madt_lapic lapic;
        acpi_madt_ioapic ioapic;
        // other things TBI
    };
} acpi_madt_entry;

#define MADT_FLAG_PIC_INSTALLED 1
#define MADT_ENTRY_LAPIC 0
#define MADT_ENTRY_IOAPIC 1

typedef struct PACKED madt {
    acpi_header header;
    u32 lapic_address;
    u32 flags;
    acpi_madt_entry entries[0];
} acpi_madt;

acpi_rsdt *acpi_get_rsdt(acpi_rsdp *rsdp);
void *acpi_get_table(int table_id);

void acpi_print_header(acpi_header *header);
void acpi_print_table(acpi_header *table);

#endif
