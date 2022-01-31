#include <basic.h>

struct gdt_pointer {
    uint16_t length;
    uint64_t address;
} __PACKED;

struct gdt_descriptor {
    uint16_t a;
    uint16_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
} __PACKED;

struct tss_descriptor {
    uint16_t a;
    uint16_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
    uint64_t g;
} __PACKED;

struct tss {
    uint32_t _reserved1;
    uint64_t rsp[3];
    uint64_t _reserved2;
    uint64_t ist[7];
    uint64_t _reserved3;
    uint16_t _reserved4;
    uint16_t iomap_base;
} __PACKED __ALIGN(0x10);

struct gdt {
    struct gdt_descriptor descriptors[6];
    struct tss_descriptor tss_descriptor;
} __PACKED __ALIGN(0x10);

struct cpu_local_tables {
    struct gdt gdt;
    struct tss tss;
};

#define KERNEL_CODE 0x9A
#define KERNEL_DATA 0x92
#define USER_CODE 0xFA
#define USER_DATA 0xF2
#define TSS 0xE9
#define LONG_MODE 0x20

extern char int_stack_top;
extern char df_stack_top;

struct cpu_local_tables cpu_tables[16] = {{
    .gdt = {
        .descriptors = {
            [0] = {0},
            [1] = { 0, 0, 0, KERNEL_CODE, LONG_MODE, 0 },
            [2] = { 0, 0, 0, KERNEL_DATA, LONG_MODE, 0 },
            [3] = { 0, 0, 0, USER_CODE, LONG_MODE, 0 },
            [4] = { 0, 0, 0, USER_DATA, LONG_MODE, 0 },
            [5] = { 0 },
        },
        .tss_descriptor = {
            sizeof(struct tss) - 1, 0, 0, TSS, LONG_MODE, 0, 0,
        },
    },
    .tss = {
        .rsp = { [0] = (uintptr_t)&int_stack_top, },
        .ist = { [0] = (uintptr_t)&df_stack_top, },
        .iomap_base = sizeof(struct tss),
    },
}};

void init_table_for_ap(int ap_number) {
}
