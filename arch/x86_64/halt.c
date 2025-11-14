#include <ng/panic.h>
#include <ng/syscalls.h>
#include <ng/x86/apic.h>
#include <ng/x86/cpu.h>
#include <ng/x86/interrupt.h>

sysret sys_haltvm(int exit_code) {
	outb(0x501, exit_code);
	printf("Stopping the VM failed\n");
	return 1;
}

[[noreturn]] void halt() {
	int cpu = cpu_id();
	for (int i = 0; i < NCPUS; i++) {
		if (i == cpu)
			continue;
		lapic_send_ipi(IPI_FIXED, 131, i);
	}
	while (true) {
		disable_irqs();
		asm volatile("pause");
		asm volatile("hlt");
	}
}
