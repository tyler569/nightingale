#include <assert.h>
#include <limine.h>
#include <ng/arch.h>
#include <ng/limine.h>
#include <ng/pmm.h>
#include <stdio.h>

void limine_init() {
	limine_memmap();
	printf("initfs address: %p\n", limine_module());
	printf("rsdp address: %p\n", limine_rsdp());
	printf("boot time: %li\n", limine_boot_time());

	printf("kernel command line: %s\n", limine_kernel_command_line());

	printf("hhdm map: %#lx\n", limine_hhdm());
}

__MUST_EMIT
static struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0,
};

void limine_memmap() {
	static const char *type_names[] = {
		[LIMINE_MEMMAP_ACPI_NVS] = "acpi",
		[LIMINE_MEMMAP_ACPI_RECLAIMABLE] = "reclaim (ac)",
		[LIMINE_MEMMAP_BAD_MEMORY] = "bad",
		[LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE] = "reclaim (bl)",
		[LIMINE_MEMMAP_FRAMEBUFFER] = "framebuffer",
		[LIMINE_MEMMAP_KERNEL_AND_MODULES] = "kernel",
		[LIMINE_MEMMAP_RESERVED] = "reserved",
		[LIMINE_MEMMAP_USABLE] = "usable",
	};

	uint64_t available_memory = 0;

	assert(memmap_request.response);
	for (int i = 0; i < memmap_request.response->entry_count; i++) {
		struct limine_memmap_entry *entry = memmap_request.response->entries[i];

		printf("%-15s %016lx %08lx\n", type_names[entry->type], entry->base,
			entry->length);

		int pm_type = PM_NOMEM;
		switch (entry->type) {
		case LIMINE_MEMMAP_USABLE:
			pm_type = PM_REF_ZERO;
			available_memory += entry->length;
			break;
		case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
		case LIMINE_MEMMAP_RESERVED:
		case LIMINE_MEMMAP_KERNEL_AND_MODULES:
		case LIMINE_MEMMAP_FRAMEBUFFER:
		case LIMINE_MEMMAP_ACPI_NVS:
		case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
			pm_type = PM_LEAK;
			break;
		}

		pm_set(entry->base, entry->base + entry->length, pm_type);
	}

	printf("available memory: %lu (%lu KB)\n", available_memory,
		available_memory / 1024);
}

__MUST_EMIT
static struct limine_module_request module_request = {
	.id = LIMINE_MODULE_REQUEST,
	.revision = 1,
};

void *limine_module() {
	assert(module_request.response);

	return module_request.response->modules[0]->address;
}

__MUST_EMIT
static struct limine_kernel_file_request kernel_file_request = {
	.id = LIMINE_KERNEL_FILE_REQUEST,
	.revision = 0,
};

void *limine_kernel_file_ptr() {
	assert(kernel_file_request.response);

	return kernel_file_request.response->kernel_file->address;
}

size_t limine_kernel_file_len() {
	assert(kernel_file_request.response);

	return kernel_file_request.response->kernel_file->size;
}

char *limine_kernel_command_line() {
	assert(kernel_file_request.response);

	return kernel_file_request.response->kernel_file->cmdline;
}

__MUST_EMIT
static struct limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0,
};

void *limine_rsdp() {
	assert(rsdp_request.response);

	return rsdp_request.response->address;
}

__MUST_EMIT
static struct limine_boot_time_request boot_time_request = {
	.id = LIMINE_BOOT_TIME_REQUEST,
	.revision = 0,
};

int64_t limine_boot_time() {
	assert(boot_time_request.response);

	return boot_time_request.response->boot_time;
}

__MUST_EMIT
static struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0,
};

virt_addr_t limine_hhdm() {
	assert(hhdm_request.response);

	return hhdm_request.response->offset;
}

__MUST_EMIT
static struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0,
};

void limine_framebuffer(uint32_t *width, uint32_t *height, uint32_t *bpp,
	uint32_t *pitch, void **address) {
	assert(framebuffer_request.response);
	assert(framebuffer_request.response->framebuffer_count == 1);

	struct limine_framebuffer *fb
		= framebuffer_request.response->framebuffers[0];
	*width = fb->width;
	*height = fb->height;
	*bpp = fb->bpp;
	*pitch = fb->pitch;
	*address = fb->address;
}
