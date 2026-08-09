#include <elf.h>
#include <limine.h>
#include <ng/init.h>
#include <ng/mman.h>
#include <ng/pmm.h>
#include <stdint.h>
#include <stdio.h>

uint64_t base_revision[3] = LIMINE_BASE_REVISION(6);

bool limine_revision_supported() {
	return base_revision[2] == 0;
}

struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST_ID,
};

struct limine_module_request module_request = {
	.id = LIMINE_MODULE_REQUEST_ID,
	.revision = 1,
};

struct limine_executable_file_request kernel_file_request = {
	.id = LIMINE_EXECUTABLE_FILE_REQUEST_ID,
};

struct limine_executable_cmdline_request cmdline_request = {
	.id = LIMINE_EXECUTABLE_CMDLINE_REQUEST_ID,
};

struct limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST_ID,
};

struct limine_date_at_boot_request boot_time_request = {
	.id = LIMINE_DATE_AT_BOOT_REQUEST_ID,
};

struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST_ID,
};

struct limine_mp_request smp_request = {
	.id = LIMINE_MP_REQUEST_ID,
};

struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST_ID,
};

static enum physical_region_type from_limine_prt(uint64_t prt) {
	switch (prt) {
	case LIMINE_MEMMAP_USABLE:
		return prt_memory;
	case LIMINE_MEMMAP_ACPI_NVS:
	case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
		return prt_acpi;
	case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
		return prt_bootloader;
	case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
		return prt_kernel;
	case LIMINE_MEMMAP_BAD_MEMORY:
	case LIMINE_MEMMAP_FRAMEBUFFER:
	case LIMINE_MEMMAP_RESERVED:
	case LIMINE_MEMMAP_RESERVED_MAPPED:
	default:
		return prt_reserved;
	}
}

static const char *type_string(enum physical_region_type prt) {
	switch (prt) {
	case prt_memory:
		return "memory";
	case prt_acpi:
		return "acpi";
	case prt_bootloader:
		return "bootloader reclaimable";
	case prt_kernel:
		return "kernel";
	case prt_reserved:
		return "reserved";
	default:
		return "unknown";
	}
}

void init_pmm() {
	struct physical_region regions[256];
	size_t n_regions = 0;

	auto resp = memmap_request.response;
	if (!resp) {
		printf("warning: no memmap provided\n");
		return;
	}

	for (size_t i = 0; i < resp->entry_count; i++) {
		auto entry = resp->entries[i];
		auto type = from_limine_prt(entry->type);

		if (type == prt_reserved)
			continue;

		regions[n_regions++] = (struct physical_region) {
			.base = entry->base,
			.len = entry->length,
			.type = type,
		};
	}

	for (size_t i = 0; i < n_regions; i++) {
		printf("%12lx %10lx %s\n", regions[i].base, regions[i].len,
			type_string(regions[i].type));
	}

	pmm_init(n_regions, regions);
}

static uintptr_t hhdm_offset() {
	static uintptr_t hhdm_offset = 0;
	if (hhdm_offset == 0)
		hhdm_offset = hhdm_request.response->offset;
	return hhdm_offset;
}

void *ptr_of(phys_addr_t addr) {
	return (void *)(addr + hhdm_offset());
}

virt_addr_t virtual_of(phys_addr_t addr) {
	return addr + hhdm_offset();
}

phys_addr_t physical_of(virt_addr_t addr) {
	return addr - hhdm_offset();
}

void *limine_module() {
	return module_request.response->modules[0]->address;
}

void *limine_rsdp() {
	return rsdp_request.response->address;
}

void *limine_kernel_file(size_t *len) {
	if (len)
		*len = kernel_file_request.response->executable_file->size;
	return kernel_file_request.response->executable_file->address;
}

void init_kernel_file() {
	size_t kernel_file_len;
	void *kernel_file_ptr = limine_kernel_file(&kernel_file_len);
	load_kernel_elf(kernel_file_ptr, kernel_file_len);
}
define_init(init_kernel_file, 3);
