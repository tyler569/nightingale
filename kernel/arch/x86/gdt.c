#include <stdlib.h>
#include <sys/cdefs.h>
#include <x86/cpu.h>
#include "x86/gdt.h"
#include "stdio.h"

void gdt_init(struct gdt_entry *gdt, struct gdt_ptr *ptr, struct tss *tss)
{
    *ptr = (struct gdt_ptr) {
        .limit = sizeof(struct gdt_entry) * 7 - 1,
        .base = (uint64_t)gdt,
    };

    gdt[0] = (struct gdt_entry) { 0 };
    gdt[1] = (struct gdt_entry) {
        .entry = {
            .access = KERNEL_CODE,
            .granularity = LONG_MODE,
        },
    };
    gdt[2] = (struct gdt_entry) {
        .entry = {
            .access = KERNEL_DATA,
            .granularity = LONG_MODE,
        },
    };
    gdt[3] = (struct gdt_entry) {
        .entry = {
            .access = USER_CODE,
            .granularity = LONG_MODE,
        },
    };
    gdt[4] = (struct gdt_entry) {
        .entry = {
            .access = USER_DATA,
            .granularity = LONG_MODE,
        },
    };
    gdt[5] = (struct gdt_entry) {
        .entry = {
            .base_low = (uint64_t)tss,
            .base_middle = (uint64_t)tss >> 16,
            .base_high = (uint64_t)tss >> 24,
            .limit_low = 0x67,
            .access = TSS,
            .granularity = 0,
        },
    };
    gdt[6] = (struct gdt_entry) {
        .entry_high = {
            .base_64 = (uint64_t)tss >> 32,
        },
    };

    tss->iomap_base = sizeof(struct tss);
    tss->rsp0 = (uint64_t)malloc(4096) + 4096;
    tss->ist1 = (uint64_t)malloc(4096) + 4096;
}

void lgdt(struct gdt_ptr *ptr)
{
    __asm__ volatile("lgdt %0" ::"m"(*ptr));
    __asm__ volatile("ltr %w0" ::"r"(0x28));
}

struct cpu {
    struct gdt_entry gdt[7];
    struct gdt_ptr gdt_ptr;
    struct tss tss;
};

__MUST_EMIT
struct cpu cpus[16];

void gdt_cpu_setup()
{
    int id = cpunum();
    struct cpu *cpu = &cpus[id];
    gdt_init(cpu->gdt, &cpu->gdt_ptr, &cpu->tss);
    printf("gdt_ptr: %p\n", &cpu->gdt_ptr);
    printf("gdt_ptr: %04x %016lx\n", cpu->gdt_ptr.limit, cpu->gdt_ptr.base);
    uint64_t *gdt = (uint64_t *)cpu->gdt;
    printf("gdt: %p\n", gdt);
    printf("gdt[0]: %02x %016lx\n", 0 * 8, gdt[0]);
    printf("gdt[1]: %02x %016lx\n", 1 * 8, gdt[1]);
    printf("gdt[2]: %02x %016lx\n", 2 * 8, gdt[2]);
    printf("gdt[3]: %02x %016lx\n", 3 * 8, gdt[3]);
    printf("gdt[4]: %02x %016lx\n", 4 * 8, gdt[4]);
    printf("gdt[5]: %02x %016lx\n", 5 * 8, gdt[5]);
    printf("gdt[6]: %02x %016lx\n", 6 * 8, gdt[6]);
    printf("tss: %p\n", &cpu->tss);
    uintptr_t ptss = (uintptr_t)&cpu->tss;
    printf("%04lx %02lx %02lx %08lx\n", ptss & 0xFFFF, (ptss >> 16) & 0xFF,
        (ptss >> 24) & 0xFF, (ptss >> 32) & 0xFFFFFFFF);

    lgdt(&cpu->gdt_ptr);
}

void set_kernel_stack(uint64_t rsp) { gdt_set_cpu_rsp0(rsp); }

void gdt_set_cpu_rsp0(uint64_t rsp0)
{
    int id = cpunum();
    cpus[id].tss.rsp0 = rsp0;
}

void gdt_set_cpu_ist1(uint64_t ist1)
{
    int id = cpunum();
    cpus[id].tss.ist1 = ist1;
}