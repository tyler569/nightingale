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

const char *banner = "\n\
********************************\n\
\n\
The Nightingale Operating System\n\
Version " NIGHTINGALE_VERSION "\n\
\n\
********************************\n\
\n\
Copyright (C) 2017-2024, Tyler Philbrick\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <https://www.gnu.org/licenses/>.\n\n";

bool print_boot_info = true;

extern struct thread thread_zero;

uint64_t tsc;

__USED
[[noreturn]] void kernel_main() {
	tsc = rdtsc();

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

	// ext2_info();

	printf("%s", banner);

	void video();
	video();

	void rbtree_test();
	rbtree_test();

	pci_address_t addr;

	if (pci_find_device_by_id(0x10ec, 0x8139) != -1) {
		void rtl_test();
		rtl_test();
	}

	if ((addr = pci_find_device_by_id(0x8086, 0x100e)) != -1) {
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

	while (true)
		__asm__ volatile("hlt");
	panic("kernel_main tried to return!");
}
