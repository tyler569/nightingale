#include <basic.h>
#include <assert.h>
#include <ng/pmm.h>
#include <stdio.h>
#include <limine.h>
#include <time.h>

void limine_memmap(void);
const char *limine_module(void);
void *limine_rsdp(void);
int64_t limine_boot_time(void);

void limine_init(void)
{
    limine_memmap();
    printf("initfs address: %p\n", limine_module());
    printf("rsdp address: %p\n", limine_rsdp());
    printf("boot time: %li\n", limine_boot_time());
}

__MUST_EMIT
static struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

void limine_memmap(void)
{
    assert(memmap_request.response);
    for (int i = 0; i < memmap_request.response->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap_request.response->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE) {
            printf("%lu %lx %lx\n", entry->type, entry->base, entry->length);

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

const char *limine_module(void)
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