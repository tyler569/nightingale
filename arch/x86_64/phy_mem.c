#include "limine.h"
#include "ng/mem.h"
#include "stdio.h"
#include "x86_64.h"
#include <ng/arch_x86_64.h>

static struct limine_memmap_request mmapinfo = {
	.id = LIMINE_MEMMAP_REQUEST,
};

static struct limine_hhdm_request hhdminfo = {
	.id = LIMINE_HHDM_REQUEST,
};

const char *limine_memmap_type_str[] = {
	[LIMINE_MEMMAP_USABLE] = "Usable",
	[LIMINE_MEMMAP_RESERVED] = "Reserved",
	[LIMINE_MEMMAP_ACPI_RECLAIMABLE] = "ACPI reclaimable",
	[LIMINE_MEMMAP_ACPI_NVS] = "ACPI NVS",
	[LIMINE_MEMMAP_BAD_MEMORY] = "Bad memory",
	[LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE] = "Bootloader reclaimable",
	[LIMINE_MEMMAP_KERNEL_AND_MODULES] = "Kernel and modules",
	[LIMINE_MEMMAP_FRAMEBUFFER] = "Framebuffer",
};

bool limine_memmap_type_available[16] = {
	[LIMINE_MEMMAP_USABLE] = true,
	[LIMINE_MEMMAP_ACPI_RECLAIMABLE] = true,
	[LIMINE_MEMMAP_ACPI_NVS] = true,
	[LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE] = true,
	[LIMINE_MEMMAP_KERNEL_AND_MODULES] = true,
};

uintptr_t direct_map_of(uintptr_t addr) {
	static uintptr_t hhdm_cache = 0;
	if (!hhdm_cache)
		hhdm_cache = volatile_get(hhdminfo.response)->offset;
	return addr | hhdm_cache;
}

uintptr_t limine_hhdm() { return direct_map_of(0); }

uintptr_t physical_of(uintptr_t addr) {
	return (uintptr_t)addr - direct_map_of(0);
}

void get_physical_extents(
	struct physical_extent *extents, size_t *extent_count) {
	size_t total_pages = 0;
	size_t free_pages = 0;

	size_t max_extents = *extent_count;
	*extent_count = 0;

	struct limine_memmap_response *resp = volatile_get(mmapinfo.response);

	printf("Limine memory map:\n");

	for (size_t i = 0; i < resp->entry_count; i++) {
		struct limine_memmap_entry *entry = resp->entries[i];

		uintptr_t top = entry->base + entry->length;
		size_t pages = entry->length / PAGE_SIZE;

		printf("  %11lx - %11lx: %s\n", entry->base, top,
			limine_memmap_type_str[entry->type]);

		if (limine_memmap_type_available[entry->type])
			total_pages += pages;

		if (entry->type == LIMINE_MEMMAP_USABLE)
			free_pages += pages;

		if (entry->type == LIMINE_MEMMAP_USABLE
			&& *extent_count < max_extents) {
			extents[*extent_count].start = entry->base;
			extents[*extent_count].len = entry->length;
			(*extent_count)++;
		}
	}
	printf("Total pages: %zu (", total_pages);
	print_si_fraction(total_pages * PAGE_SIZE);
	printf(")\n");
	printf("Free pages: %zu (", free_pages);
	print_si_fraction(free_pages * PAGE_SIZE);
	printf(")\n");
}
