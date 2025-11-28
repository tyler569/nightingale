#include <elf.h>
#include <ng/arch.h>
#include <ng/commandline.h>
#include <ng/debug.h>
#include <ng/event_log.h>
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

struct tar_header *initfs;
int have_fsgsbase = 0;
bool initialized = false;

const char banner[] = {
#embed "banner.txt"
};

bool print_boot_info = true;

extern struct thread thread_zero;

[[noreturn]] void real_main();
[[noreturn]] void ap_kernel_main();
extern char hhstack_top;

uint64_t tsc;

[[noreturn]] void kernel_main() {
	tsc = rdtsc();

	tty_init();

	// pm_init(); -> arch.c
	// limine_init(); -> arch.c

	random_add_boot_randomness();
	event_log_init();
	timer_init();

	void *kernel_file_ptr = limine_kernel_file_ptr();
	size_t kernel_file_len = limine_kernel_file_len();
	limine_load_kernel_elf(kernel_file_ptr, kernel_file_len);

	initfs = (struct tar_header *)limine_module();
	fs_init(initfs);
	threads_init();

	if (print_boot_info)
		pci_enumerate_bus_and_print();

	procfs_init();
	run_all_tests();

	initialized = true;

	const char *init_program = get_kernel_argument("init");
	if (!init_program)
		init_program = "/bin/init";
	bootstrap_usermode(init_program);

	printf("\n%s\n", banner);
	printf("(version %s)\n", NIGHTINGALE_VERSION);

	void video();
	video();

	void rbtree_test();
	rbtree_test();

	pci_address_t addr;

	if (pci_find_device_by_id(0x10ec, 0x8139) != ~0u) {
		void rtl_test();
		rtl_test();
	}

	if ((addr = pci_find_device_by_id(0x8086, 0x100e)) != ~0u) {
		void e1000_test(pci_address_t);
		e1000_test(addr);
	}

	void print_test();
	print_test();

	void net_test();
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
