#include <assert.h>
#include <ng/arch.h>
#include <ng/arch/x86_64/acpi.h>
#include <ng/arch/x86_64/apic.h>
#include <ng/arch/x86_64/cpu.h>
#include <ng/arch/x86_64/gdt.h>
#include <ng/arch/x86_64/interrupt.h>
#include <ng/arch/x86_64/pic.h>
#include <ng/init.h>
#include <ng/limine.h>
#include <ng/mman.h>
#include <ng/pmm.h>
#include <ng/thread.h>
#include <stdlib.h>

static void cpu_feat_setup() {
	disable_bits_cr0(1 << 2); // CR0.EM
	enable_bits_cr0(1 << 1); // CR0.MP
	enable_bits_cr4(3 << 9); // CR4.OSFXSR and CR4.OSXMMEXCPT

	if (supports_feature(_X86_FSGSBASE)) {
		enable_bits_cr4(1 << 16); // enable fsgsbase
	}
}
define_init(cpu_feat_setup, 0);

static void gdt_init() {
	gdt_cpu_setup(0);
	gdt_cpu_load();
}
define_init(gdt_init, 0);

static void idt_init() {
	idt_install();
	idt_load();
}
define_init(idt_init, 0);

static void early_heap_init() {
	heap_init(__global_heap_ptr, early_malloc_pool, EARLY_MALLOC_POOL_LEN);
}
define_init(early_heap_init, 0);

static void cpu_local_init() {
	extern unsigned char percpu_template_start[], percpu_template_end[];
	size_t len = percpu_template_end - percpu_template_start;
	void *percpu_region = malloc(len);
	*(uintptr_t *)percpu_region = (uintptr_t)percpu_region; // self-pointer
	memcpy(percpu_region, percpu_template_start, len);

	set_gs_base(percpu_region - (void *)percpu_template_start);
}
define_init(cpu_local_init, 1);

static void ic_init() {
	acpi_rsdp_t *rsdp = limine_rsdp();
	acpi_init(rsdp);
	void *madt = acpi_get_table("APIC");
	assert(madt);

	pic_init();
	ioapic_init(madt);
	lapic_init();
}
define_init(ic_init, 2);

void arch_ap_setup(int cpu) {
	gdt_cpu_setup(cpu);
}

void arch_ap_init() {
	cpu_feat_setup();

	gdt_cpu_load();
	idt_load();

	lapic_init();
}

void arch_thread_context_save(struct thread *th) {
	asm volatile("fxsaveq %0" : : "m"(th->fpctx));
}

void arch_thread_context_restore(struct thread *th) {
	asm volatile("fxrstorq %0" : : "m"(th->fpctx));
}

void arch_enable_irqs() {
	asm volatile("sti");
}

void arch_disable_irqs() {
	asm volatile("cli");
}

uint64_t arch_save_irqs() {
	uint64_t flags;
	asm volatile("pushfq\n\t"
				 "pop %0\n\t"
				 "cli"
		: "=rm"(flags)
		:
		: "memory");
	return flags;
}

void arch_restore_irqs(uint64_t flags) {
	asm volatile("push %0\n\t"
				 "popfq"
		:
		: "rm"(flags)
		: "memory");
}

[[noreturn]] void arch_halt_forever() {
	while (true)
		asm volatile("hlt");
}
