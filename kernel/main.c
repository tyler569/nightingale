#include <elf.h>
#include <ng/arch.h>
#include <ng/init.h>
#include <ng/panic.h>
#include <ng/x86/cpu.h>
#include <stdio.h>
#include <stdlib.h>
#include <version.h>

const char banner[] = {
#embed "banner.txt"
};

[[noreturn]] void kernel_main() {
	uint64_t tsc = rdtsc();

	printf("\n%s\n", banner);
	printf("(version %s)\n", NIGHTINGALE_VERSION);

	do_init_calls();

	printf("initialization took: %li\n", rdtsc() - tsc);

	enable_irqs();
	// limine_smp_init((limine_goto_address)ap_kernel_main);

	while (true)
		asm volatile("hlt");
	panic("kernel_main tried to return!");
}

[[noreturn]] void ap_kernel_main() {
	printf("\nthis is the application processor\n");
	arch_ap_init();
	printf("lapic: initialized\n");
	for (;;) {
		asm volatile("hlt");
	}
}
