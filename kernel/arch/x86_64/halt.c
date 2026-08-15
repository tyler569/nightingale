#include <ng/arch/x86_64/apic.h>
#include <ng/arch/x86_64/cpu.h>
#include <ng/arch/x86_64/interrupt.h>
#include <ng/panic.h>
#include <ng/syscalls.h>

sysret sys_haltvm(int exit_code) {
	outb(0x501, exit_code);
	printf("Stopping the VM failed\n");
	return 1;
}

[[noreturn]] void halt() {
	int cpu = cpu_id();
	// TODO: later replace the send_ipi code with a broadcast or like
	// a lock or something
	while (true) {
		asm volatile("cli");
		asm volatile("pause");
		asm volatile("hlt");
	}
}
