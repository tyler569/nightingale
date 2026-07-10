#include <elf.h>
#include <ng/arch.h>
#include <ng/commandline.h>
#include <ng/debug.h>
#include <ng/fs/init.h>
#include <ng/limine.h>
#include <ng/panic.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <ng/proc_files.h>
#include <ng/random.h>
#include <ng/serial.h>
#include <ng/tarfs.h>
#include <ng/tests.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/tty.h>
#include <ng/x86/acpi.h>
#include <ng/x86/cpu.h>
#include <ng/x86/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <version.h>

const char banner[] = {
#embed "banner.txt"
};

[[noreturn]] void ap_kernel_main();
void video();
void rbtree_test();
void rtl_test();
void e1000_test(pci_address_t);
void print_test();
void net_test();

[[noreturn]] void kernel_main() {
	uint64_t tsc = rdtsc();

	tty_init();

	random_add_boot_randomness();
	timer_init();

	size_t kernel_file_len;
	void *kernel_file_ptr = limine_kernel_file(&kernel_file_len);
	load_kernel_elf(kernel_file_ptr, kernel_file_len);

	struct tar_header *initfs = limine_module();
	fs_init(initfs);
	threads_init();

	pci_enumerate_bus_and_print();

	procfs_init();
	run_all_tests();

	const char *init_program = get_kernel_argument("init");
	if (!init_program)
		init_program = "/bin/init";
	bootstrap_usermode(init_program);

	printf("\n%s\n", banner);
	printf("(version %s)\n", NIGHTINGALE_VERSION);

	video();

	rbtree_test();

	pci_address_t addr;

	if (pci_find_device_by_id(0x10ec, 0x8139) != ~0u)
		rtl_test();

	if ((addr = pci_find_device_by_id(0x8086, 0x100e)) != ~0u)
		e1000_test(addr);

	print_test();

	net_test();

	printf("threads: usermode thread installed\n");
	printf("initialization took: %li\n", rdtsc() - tsc);
	printf("cpu: allowing irqs\n");

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
