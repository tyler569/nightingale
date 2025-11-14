#include <assert.h>
#include <ng/arch.h>
#include <ng/limine.h>
#include <ng/thread.h>
#include <ng/x86/acpi.h>
#include <ng/x86/apic.h>
#include <ng/x86/cpu.h>
#include <ng/x86/gdt.h>
#include <ng/x86/interrupt.h>
#include <ng/x86/pic.h>

static void cpu_feat_setup() {
	disable_bits_cr0(1 << 2); // CR0.EM
	enable_bits_cr0(1 << 1); // CR0.MP
	enable_bits_cr4(3 << 9); // CR4.OSFXSR and CR4.OSXMMEXCPT

	if (supports_feature(_X86_FSGSBASE)) {
		enable_bits_cr4(1 << 16); // enable fsgsbase
	}
}

void arch_init() {
	cpu_feat_setup();

	gdt_cpu_setup(0);
	gdt_cpu_load();
	set_gs_base(thread_cpus[0]);
	idt_install();
	idt_load();

	heap_init(__global_heap_ptr, early_malloc_pool, EARLY_MALLOC_POOL_LEN);

	serial_init();

	uint64_t tmp;
	asm volatile("mov %%cr3, %0" : "=a"(tmp));
	running_process->vm_root = tmp & 0x00FFFFFFFFFFF000;

	acpi_rsdp_t *rsdp = limine_rsdp();
	acpi_init(rsdp);
	void *madt = acpi_get_table("APIC");
	assert(madt);

	pic_init();

	ioapic_init(madt);
	lapic_init();
}

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
