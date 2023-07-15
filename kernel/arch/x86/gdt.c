#include <sys/cdefs.h>
#include "gdt.h"

__USED static struct gdt_entry gdt[5];

void init_gdt(void)
{
    struct gdt_ptr ptr = {
        .limit = sizeof(gdt) - 1,
        .base = (uint64_t)gdt,
    };

    gdt[0] = (struct gdt_entry) {
        .limit_low = 0,
        .base_low = 0,
        .base_middle = 0,
        .access = 0,
        .granularity = 0,
        .base_high = 0,
    };

    gdt[1] = (struct gdt_entry) {
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_middle = 0,
        .access = 0x9A,
        .granularity = 0xCF,
        .base_high = 0,
    };

    gdt[2] = (struct gdt_entry) {
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_middle = 0,
        .access = 0x92,
        .granularity = 0xCF,
        .base_high = 0,
    };

    gdt[3] = (struct gdt_entry) {
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_middle = 0,
        .access = 0xFA,
        .granularity = 0xCF,
        .base_high = 0,
    };

    gdt[4] = (struct gdt_entry) {
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_middle = 0,
        .access = 0xF2,
        .granularity = 0xCF,
        .base_high = 0,
    };

    __asm__ volatile("lgdt %0" ::"m"(ptr));
    __asm__ volatile("mov $0x10, %ax");
    __asm__ volatile("mov %ax, %ds");
    __asm__ volatile("mov %ax, %es");
    __asm__ volatile("mov %ax, %fs");
    __asm__ volatile("mov %ax, %gs");
    __asm__ volatile("mov %ax, %ss");
    __asm__ volatile("ljmp $0x08, $__j1");
    __asm__ volatile("__j1:");
}