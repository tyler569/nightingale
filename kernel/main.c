#include <elf.h>
#include <ng/arch.h>
#include <ng/cpu.h>
#include <ng/init.h>
#include <ng/panic.h>
#include <ng/per_cpu.h>
#include <stdio.h>
#include <stdlib.h>
#include <version.h>

const char banner[] = {
#embed "banner.txt"
	,
	0,
};

cpu_local unsigned long foo = 10;

[[noreturn]] void kernel_main() {
	uint64_t tsc = rdtsc();

	do_init_calls();

	printf("\n%*s\n", (int)sizeof(banner), banner);
	printf("(version %s)\n", NIGHTINGALE_VERSION);
	printf("initialization took: %li\n", rdtsc() - tsc);

	printf("%p %lx\n", &foo, cpu_ref(foo));

	arch_enable_irqs();

	void rust_test(int value);
	rust_test(4567);

	// limine_smp_init((limine_goto_address)ap_kernel_main);

	arch_halt_forever();

	panic("kernel_main tried to return!");
}

void c_panic() {
	panic();
}

[[noreturn]] void ap_kernel_main() {
	printf("\nthis is the application processor\n");
	arch_ap_init();
	printf("lapic: initialized\n");

	arch_halt_forever();
}
