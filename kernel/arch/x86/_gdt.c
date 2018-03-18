
#include <basic.h>
// #include "gdt.h"

#define KERNEL_CODE 0x9A
#define KERNEL_DATA 0x92
#define USER_CODE   0xFA
#define USER_DATA   0xF2
#define TSS         0xE9

#define LONG_MODE 0x40

#define GDT_ENTRY_NULL {0}
#define GDT_ENTRY(type_, flags_) { .type = type_, .flags = flags_ }
#define GDT_ENTRY64(base, limit, type, flags) \
    {   \
        (limit) & 0xFFFF, (base) & 0xFFFF, ((base) >> 16) & 0xFFFF, \
        (type), (((limit) >> 16) & 0x0F) | (flags), ((base) >> 24) & 0xFFFF, \
    },  \
    {   \
        ((base) >> 32) & 0xFFFF, ((base) >> 48 & 0xFFFF) \
    }

    
    
struct __packed gdt_entry {
    uint16_t ignored0;
    uint16_t ignored1;
    uint8_t ignored2;
    uint8_t type;
    uint8_t flags;
    uint8_t ignored3;
};

struct __packed gdt_ptr {
    uint16_t len;
    uint64_t ptr;
};

struct __packed tss {
    uint32_t reserved0;
    uint64_t stack_pl0;
    uint64_t stack_pl1;
    uint64_t stack_pl2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap;
};

struct tss kernel_tss = {};

struct gdt_entry gdt[] = {
    GDT_ENTRY_NULL,
    GDT_ENTRY(KERNEL_CODE, LONG_MODE),
    GDT_ENTRY(USER_CODE, LONG_MODE),
    GDT_ENTRY(USER_DATA, LONG_MODE),
    GDT_ENTRY64((uint64_t)&kernel_tss, sizeof(struct tss) - 1, TSS, LONG_MODE),
};

struct gdt_ptr gdtp = {
    sizeof(gdt) - 1,
    &gdt,
};

