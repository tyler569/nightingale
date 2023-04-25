#include <basic.h>
#include <assert.h>
#include <ng/pmm.h>
#include <stdio.h>
#include <limine.h>
#include <time.h>

void limine_memmap(void);
void *limine_module(void);
void *limine_rsdp(void);
int64_t limine_boot_time(void);
phys_addr_t limine_kernel_physical_base(void);
virt_addr_t limine_kernel_virtual_base(void);
virt_addr_t limine_hhdm(void);

void limine_init(void)
{
    limine_memmap();
    printf("initfs address: %p\n", limine_module());
    printf("rsdp address: %p\n", limine_rsdp());
    printf("boot time: %li\n", limine_boot_time());

    printf("kernel virtual base: %#lx\n", limine_kernel_virtual_base());
    printf("kernel physical base: %#lx\n", limine_kernel_physical_base());
    printf("hhdm map: %#lx\n", limine_hhdm());
}

__MUST_EMIT
static struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

void limine_memmap(void)
{
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

    assert(memmap_request.response);
    for (int i = 0; i < memmap_request.response->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap_request.response->entries[i];

        printf("%-15s %016lx %08lx\n", type_names[entry->type], entry->base,
            entry->length);

        if (entry->type == LIMINE_MEMMAP_USABLE
            || entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
            pm_set(entry->base, entry->base + entry->length, PM_REF_ZERO);
        } else {
            pm_set(entry->base, entry->base + entry->length, PM_LEAK);
        }
    }
}

__MUST_EMIT
static struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0,
    .flags = LIMINE_SMP_X2APIC,
};

static struct limine_internal_module tarfs_module = {
    .cmdline = "",
    .flags = LIMINE_INTERNAL_MODULE_REQUIRED,
    .path = "initfs.tar",
};

__MUST_EMIT
static struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 1,
    .internal_module_count = 1,
    .internal_modules = (struct limine_internal_module *[]) { &tarfs_module },
};

void *limine_module(void)
{
    assert(module_request.response);

    return module_request.response->modules[0]->address;
}

__MUST_EMIT
static struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
};

void *limine_rsdp(void)
{
    assert(rsdp_request.response);

    return rsdp_request.response->address;
}

__MUST_EMIT
static struct limine_boot_time_request boot_time_request = {
    .id = LIMINE_BOOT_TIME_REQUEST,
    .revision = 0,
};

int64_t limine_boot_time(void)
{
    assert(boot_time_request.response);

    return boot_time_request.response->boot_time;
}

__MUST_EMIT
static struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

phys_addr_t limine_kernel_physical_base(void)
{
    assert(kernel_address_request.response);

    return kernel_address_request.response->physical_base;
}

virt_addr_t limine_kernel_virtual_base(void)
{
    assert(kernel_address_request.response);

    return kernel_address_request.response->virtual_base;
}

__MUST_EMIT
static struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
};

virt_addr_t limine_hhdm(void)
{
    assert(hhdm_request.response);

    return hhdm_request.response->offset;
}