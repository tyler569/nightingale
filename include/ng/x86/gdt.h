#pragma once
#ifndef NIGHTINGALE_GDT_H
#define NIGHTINGALE_GDT_H

#include "stdint.h"

#define KERNEL_CODE 0x9A
#define KERNEL_DATA 0x92
#define USER_CODE 0xFA
#define USER_DATA 0xF2
#define TSS 0x89
#define LONG_MODE 0x20

struct gdt_entry {
	union {
		struct {
			uint16_t limit_low;
			uint16_t base_low;
			uint8_t base_middle;
			uint8_t access;
			uint8_t granularity;
			uint8_t base_high;
		} entry;
		struct {
			uint64_t base_64;
		} entry_high;
	};
} __attribute__((packed));

struct gdt_ptr {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

struct tss {
	uint32_t reserved0_1;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t reserved0_2;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t reserved0_3;
	uint16_t reserved0_4;
	uint16_t iomap_base;
} __attribute__((packed));

_Static_assert(sizeof(struct gdt_entry) == 8, "gdt_entry is not 8 bytes");
_Static_assert(sizeof(struct gdt_ptr) == 10, "gdt_ptr is not 10 bytes");
_Static_assert(sizeof(struct tss) == 104, "tss is not 104 bytes");

void gdt_cpu_setup();
void gdt_set_cpu_rsp0(uint64_t rsp0);
void gdt_set_cpu_ist1(uint64_t ist1);

#endif // NIGHTINGALE_GDT_H
