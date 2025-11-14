#include <ng/x86/gdt.h>
#include <stdint.h>
#include <string.h>

#define NCPUS 32

struct __PACKED gdt_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
};

struct __PACKED tss_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
	uint32_t base_upper;
	uint32_t reserved;
};

struct __PACKED tss {
	uint32_t reserved0;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t reserved1;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t iopb_offset;
};

struct __PACKED gdt {
	struct gdt_entry null;
	struct gdt_entry kernel_code;
	struct gdt_entry kernel_data;
	struct gdt_entry user_code;
	struct gdt_entry user_data;
	struct tss_entry tss;
};

struct __PACKED gdt_ptr {
	uint16_t limit;
	uint64_t base;
};

static struct gdt gdts[NCPUS];
static struct tss tsses[NCPUS];

static void set_gdt_entry(struct gdt_entry *entry, uint32_t base,
	uint32_t limit, uint8_t access, uint8_t granularity) {
	entry->base_low = base & 0xffff;
	entry->base_mid = (base >> 16) & 0xff;
	entry->base_high = (base >> 24) & 0xff;
	entry->limit_low = limit & 0xffff;
	entry->granularity = (limit >> 16) & 0x0f;
	entry->granularity |= granularity & 0xf0;
	entry->access = access;
}

static void set_tss_entry(struct tss_entry *entry, uint64_t base,
	uint32_t limit, uint8_t access, uint8_t granularity) {
	entry->base_low = base & 0xffff;
	entry->base_mid = (base >> 16) & 0xff;
	entry->base_high = (base >> 24) & 0xff;
	entry->base_upper = (base >> 32) & 0xffffffff;
	entry->limit_low = limit & 0xffff;
	entry->granularity = (limit >> 16) & 0x0f;
	entry->granularity |= granularity & 0xf0;
	entry->access = access;
	entry->reserved = 0;
}

void gdt_cpu_setup(int cpu) {
	struct gdt *gdt = &gdts[cpu];
	struct tss *tss = &tsses[cpu];

	memset(gdt, 0, sizeof(struct gdt));
	memset(tss, 0, sizeof(struct tss));

	// null descriptor
	set_gdt_entry(&gdt->null, 0, 0, 0, 0);

	// kernel code: base=0, limit=0xfffff, access=0x9a, granularity=0xa0
	set_gdt_entry(&gdt->kernel_code, 0, 0xfffff, 0x9a, 0xa0);

	// kernel data: base=0, limit=0xfffff, access=0x92, granularity=0xc0
	set_gdt_entry(&gdt->kernel_data, 0, 0xfffff, 0x92, 0xc0);

	// user code: base=0, limit=0xfffff, access=0xfa, granularity=0xa0
	set_gdt_entry(&gdt->user_code, 0, 0xfffff, 0xfa, 0xa0);

	// user data: base=0, limit=0xfffff, access=0xf2, granularity=0xc0
	set_gdt_entry(&gdt->user_data, 0, 0xfffff, 0xf2, 0xc0);

	// tss entry
	uint64_t tss_base = (uint64_t)tss;
	uint32_t tss_limit = sizeof(struct tss) - 1;
	set_tss_entry(&gdt->tss, tss_base, tss_limit, 0x89, 0x00);

	tss->iopb_offset = sizeof(struct tss);
}

void gdt_cpu_load() {
	struct gdt_ptr gdt_ptr;
	gdt_ptr.limit = sizeof(struct gdt) - 1;
	gdt_ptr.base = (uint64_t)&gdts[0]; // FIXME: use cpu id

	asm volatile("lgdt %0" : : "m"(gdt_ptr));

	// reload segment registers
	asm volatile("push $0x08\n\t"
				 "lea 1f(%%rip), %%rax\n\t"
				 "push %%rax\n\t"
				 "lretq\n\t"
				 "1:\n\t"
		:
		:
		: "rax");

	asm volatile("mov $0x0, %%ax\n\t"
				 "mov %%ax, %%ds\n\t"
				 "mov %%ax, %%es\n\t"
				 "mov %%ax, %%fs\n\t"
				 "mov %%ax, %%gs\n\t"
				 "mov %%ax, %%ss\n\t"
		:
		:
		: "ax");

	// load TSS
	asm volatile("ltr %%ax" : : "a"(0x28));
}

void set_kernel_stack(void *sp) {
	tsses[0].rsp0 = (uint64_t)sp; // FIXME: use cpu id
}
