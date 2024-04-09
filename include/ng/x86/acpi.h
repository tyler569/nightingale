#pragma once

#include "stdint.h"
#include "sys/cdefs.h"

#define RSDP_SIGNATURE "RSD PTR "
#define RSDT_SIGNATURE "RSDT"
#define FADT_SIGNATURE "FACP"
#define SSDT_SIGNATURE "SSDT"
#define MADT_SIGNATURE "APIC"

typedef struct __PACKED acpi_rsdp {
	char signature[8];
	unsigned char checksum;
	char oem_id[6];
	unsigned char revision;
	uint32_t rsdt_address;
} acpi_rsdp_t;

typedef struct __PACKED acpi_header {
	char signature[4];
	uint32_t length;
	unsigned char revision;
	unsigned char checksum;
	char oem_id[6];
	char oem_table_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} acpi_header_t;

typedef struct __PACKED acpi_rsdt {
	struct acpi_header header;
	uint32_t table_ptr[];
} acpi_rsdt_t;

#define MADT_LAPIC_FLAG_PROCESSOR_ENABLED 1
typedef struct __PACKED acpi_madt_lapic {
	unsigned char processor_id;
	unsigned char id;
	uint32_t flags;
} acpi_madt_lapic_t;

typedef struct __PACKED acpi_madt_ioapic {
	unsigned char id;
	unsigned char _reserved_0;
	uint32_t address;
	uint32_t interrupt_base;
} acpi_madt_ioapic_t;

typedef struct __PACKED acpi_madt_iso {
	unsigned char bus_source;
	unsigned char irq_source;
	uint32_t global_system_interrupt;
	uint16_t flags;
} acpi_madt_iso_t;

typedef struct __PACKED acpi_madt_nmi {
	unsigned char processor_id;
	uint16_t flags;
	unsigned char LINT_number;
} acpi_madt_nmi_t;

typedef struct __PACKED acpi_madt_lapic_address {
	uint16_t _reserved_0;
	uint64_t address;
} acpi_madt_lapic_address_t;

typedef struct __PACKED acpi_madt_entry {
	unsigned char type;
	unsigned char length;
	union {
		struct acpi_madt_lapic lapic;
		struct acpi_madt_ioapic ioapic;
		struct acpi_madt_iso iso;
		struct acpi_madt_nmi nmi;
		struct acpi_madt_lapic_address lapic_address;
	};
} acpi_madt_entry_t;

#define MADT_FLAG_PIC_INSTALLED 1

#define MADT_ENTRY_LAPIC 0
#define MADT_ENTRY_IOAPIC 1
#define MADT_ENTRY_ISO 2
#define MADT_ENTRY_NMI 4
#define MADT_ENTRY_LAPIC_ADDRESS 5

typedef struct __PACKED madt {
	struct acpi_header header;
	uint32_t lapic_address;
	uint32_t flags;
	struct acpi_madt_entry entries[];
} acpi_madt_t;

BEGIN_DECLS

void acpi_init(acpi_rsdp_t *rsdp);

acpi_rsdt_t *acpi_rsdt(acpi_rsdp_t *rsdp);
#define acpi_get_rsdt acpi_rsdt
void *acpi_get_table(const char *table_id);

void acpi_print_rsdp(acpi_rsdp_t *header);
void acpi_print_header(acpi_header_t *header);
void acpi_print_table(acpi_header_t *table);

END_DECLS
