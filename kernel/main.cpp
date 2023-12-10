#include <elf.h>
#include <ng/arch.h>
#include <ng/commandline.h>
#include <ng/debug.h>
#include <ng/drv/pci_device.h>
#include <ng/drv/rtl8139.h>
#include <ng/event_log.h>
#include <ng/fs/init.h>
#include <ng/limine.h>
#include <ng/mt/process.h>
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
#include <nx/atomic.h>
#include <nx/functional.h>
#include <nx/list.h>
#include <nx/memory.h>
#include <nx/print.h>
#include <nx/string.h>
#include <nx/vector.h>
#include <stdio.h>
#include <stdlib.h>
#include <version.h>

tar_header *initfs;
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
Copyright (C) 2017-2023, Tyler Philbrick\n\
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

extern thread thread_zero;

[[noreturn]] void real_main();
extern char hhstack_top;

extern "C" void early_init(void)
{
    set_gs_base(thread_cpus[0]);
    idt_install();

    heap_init(__global_heap_ptr, early_malloc_pool, EARLY_MALLOC_POOL_LEN);
    serial_init();

    nx::print("vm_root: %\n", running_process->vm_root);

    arch_init();
    acpi_init(reinterpret_cast<acpi_rsdp_t *>(limine_rsdp()));

    tty_init();
    pm_init();
    limine_init();
}

uint64_t tsc;

__USED
extern "C" [[noreturn]] void kernel_main(void)
{
    tsc = rdtsc();

    early_init();

    random_dance();
    event_log_init();
    timer_init();
    longjump_kcode((uintptr_t)real_main, (uintptr_t)&hhstack_top);
}

[[noreturn]] void real_main()
{
    printf("real_main\n");

    void *kernel_file_ptr = limine_kernel_file_ptr();
    size_t kernel_file_len = limine_kernel_file_len();
    limine_load_kernel_elf(kernel_file_ptr, kernel_file_len);

    initfs = (tar_header *)limine_module();
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

    if (print_boot_info) {
        printf("threads: usermode thread installed\n");
        printf("initialization took: %li\n", rdtsc() - tsc);
        printf("cpu: allowing irqs\n");
    }

    enable_irqs();

    void cpp_test();
    cpp_test();

    void ap_kernel_main();
    limine_smp_init(1, reinterpret_cast<limine_goto_address>(ap_kernel_main));

    while (true)
        __asm__ volatile("hlt");
    panic("kernel_main tried to return!");
}

void ap_kernel_main()
{
    printf("\nthis is the application processor\n");
    arch_ap_init();
    printf("lapic: initialized\n");
}
