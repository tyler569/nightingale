#include <ng/debug.h>
#include <ng/string.h>
#include <ng/vmm.h>
#include <ng/x86/acpi.h>
#include <stdlib.h>
#include <vec.h>

static acpi_rsdp_t *rsdp;
static acpi_rsdt_t *rsdt;
static vec(acpi_header_t *) mappings;

void acpi_init(acpi_rsdp_t *hw_rsdp) {
	rsdp = hw_rsdp;

	rsdt = (acpi_rsdt_t *)(rsdp->rsdt_address + HW_MAP_BASE);
	int table_count = ((rsdt->header.length - sizeof(rsdt->header)) / 4);

	for (int i = 0; i < table_count; i++) {
		vec_push(
			&mappings, (acpi_header_t *)(rsdt->table_ptr[i] + HW_MAP_BASE));
	}
}

acpi_rsdt_t *acpi_rsdt(acpi_rsdp_t *rsdp) {
	if (!rsdt)
		acpi_init(rsdp);
	return rsdt;
}

void *acpi_get_table(const char *table_id) {

	vec_foreach(&mappings) {
		acpi_header_t *header = *it;
		if (memcmp(header->signature, table_id, 4) == 0) {
			return header;
		}
	}
	return nullptr;
}

void acpi_print_rsdp(acpi_rsdp_t *rsdp) {
	printf("acpi rsdp @ %p {\n"
		   "\tsignature: '%.8s'\n"
		   "\tchecksum:  %#04hhX\n"
		   "\toem:       '%.6s'\n"
		   "\trevision:  %hhi\n"
		   "\trsdt:      %#010X\n"
		   "}\n",
		(void *)rsdp, rsdp->signature, rsdp->checksum, rsdp->oem_id,
		rsdp->revision, rsdp->rsdt_address);
}

void acpi_print_header(acpi_header_t *header) {
	printf("\tsignature:    '%.4s'\n"
		   "\tlength:       %u\n"
		   "\trevision:     %hhu\n"
		   "\tchecksum:     %#04hhX\n"
		   "\toem:          '%.6s'\n"
		   "\toem table:    '%.8s'\n"
		   "\toem revision: %u\n"
		   "\tcreator id:   %u ('%.4s')\n"
		   "\tcreator rev:  %u\n",
		header->signature, header->length, header->revision, header->checksum,
		header->oem_id, header->oem_table_id, header->oem_revision,
		header->creator_id, (char *)&header->creator_id,
		header->creator_revision);
}

void acpi_print_rsdt_tables(acpi_rsdt_t *rsdt) {
	printf("\ttables: [\n");
	vec_foreach(&mappings) {
		acpi_header_t *header = *it;
		printf("\t\t%.4s\n", header->signature);
	}
	printf("\t]\n");
}

const char *madt_type_names[] = {
	[MADT_ENTRY_LAPIC] = "lapic",
	[MADT_ENTRY_IOAPIC] = "ioapic",
	[MADT_ENTRY_ISO] = "iso",
	[MADT_ENTRY_NMI] = "nmi",
	[MADT_ENTRY_LAPIC_ADDRESS] = "lapic address",
};

void acpi_print_madt(acpi_madt_t *madt) {
	printf("\tlapic addr:   %#010X\n"
		   "\tflags:        %#010X\n"
		   "\tentries: [\n",
		madt->lapic_address, madt->flags);
	unsigned length = offsetof(acpi_madt_t, entries);
	while (length < madt->header.length) {
		acpi_madt_entry_t *entry = PTR_ADD(madt, length);
		printf("\t{\n"
			   "\t\ttype:           %s\n"
			   "\t\tlength:         %hhu\n",
			madt_type_names[entry->type], entry->length);
		switch (entry->type) {
		case MADT_ENTRY_LAPIC:
			printf("\t\tprocessor id:   %hhu\n"
				   "\t\tid:             %hhu\n"
				   "\t\tflags:          %#010X\n",
				entry->lapic.processor_id, entry->lapic.id, entry->lapic.flags);
			break;
		case MADT_ENTRY_IOAPIC:
			printf("\t\tid:             %hhu\n"
				   "\t\taddress:        %#010X\n"
				   "\t\tinterrupt_base: %u\n",
				entry->ioapic.id, entry->ioapic.address,
				entry->ioapic.interrupt_base);
			break;
		case MADT_ENTRY_ISO:
			printf("\t\tbus source:     %hhu\n"
				   "\t\tirq source:     %hhu\n"
				   "\t\tglobal int:     %u\n"
				   "\t\tflags:          %#06hX\n",
				entry->iso.bus_source, entry->iso.irq_source,
				entry->iso.global_system_interrupt, entry->iso.flags);
			break;
		case MADT_ENTRY_NMI:
			printf("\t\tprocessor id:   %hhu\n"
				   "\t\tflags:          %#06hX\n"
				   "\t\tlint number:    %hhu\n",
				entry->nmi.processor_id, entry->nmi.flags,
				entry->nmi.LINT_number);
			break;
		case MADT_ENTRY_LAPIC_ADDRESS:
			printf(
				"\t\taddress:        %#018lX\n", entry->lapic_address.address);
			break;
		default:
			printf("\t\t(unknown)\n");
		}
		printf("\t}\n");
		length += entry->length;
	}
	printf("\t]\n");
}

void acpi_print_table(acpi_header_t *header) {
	printf("acpi %.4s @ %p {\n", header->signature, (void *)header);
	acpi_print_header(header);
	if (memcmp(header->signature, "APIC", 4) == 0) {
		acpi_print_madt((acpi_madt_t *)header);
	}
	if (memcmp(header->signature, "RSDT", 4) == 0) {
		acpi_print_rsdt_tables((acpi_rsdt_t *)header);
	}
	printf("}\n");
}
