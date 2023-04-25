#include <basic.h>
#include <ng/commandline.h>
#include <ng/debug.h>
#include <ng/event_log.h>
#include <ng/fs.h>
#include <ng/multiboot.h>
#include <ng/multiboot2.h>
#include <ng/panic.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <ng/random.h>
#include <ng/serial.h>
#include <ng/tarfs.h>
#include <ng/tests.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/vmm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <elf.h>
#include <version.h>
#include <x86/acpi.h>
#include <x86/apic.h>
#include <x86/cpu.h>
#include <x86/interrupt.h>
#include <x86/pic.h>

struct tar_header *initfs;
int have_fsgsbase = 0;
bool initialized = false;

void mb_pm_callback(phys_addr_t mem, size_t len, int type)
{
    int pm_type;
    if (type == MULTIBOOT_MEMORY_AVAILABLE) {
        pm_type = PM_REF_ZERO;
    } else {
        pm_type = PM_LEAK;
    }
    pm_set(mem, mem + len, pm_type);
}

void proc_test(struct file *ofd, void *_)
{
    proc_sprintf(ofd, "Hello World\n");
}

void procfs_init()
{
    extern void timer_procfile(struct file *, void *);
    extern void proc_syscalls(struct file *, void *);
    extern void proc_mods(struct file *, void *);
    extern void pm_summary(struct file *, void *);
    extern void proc_heap(struct file * file, void *_);

    make_proc_file("test", proc_test, NULL);
    make_proc_file("timer", timer_procfile, NULL);
    make_proc_file("mem", pm_summary, NULL);
    make_proc_file("syscalls", proc_syscalls, NULL);
    make_proc_file("mods", proc_mods, NULL);
    make_proc_file("heap", proc_heap, NULL);

    struct dentry *ddir = proc_file_system->root;
    extern struct inode_operations proc_self_ops;
    struct inode *inode = new_inode(proc_file_system, _NG_SYMLINK | 0444);
    inode->ops = &proc_self_ops;
    add_child(ddir, "self", inode);
}

extern char _kernel_phy_base;
extern char _kernel_phy_top;

const char *banner = "\n\
********************************\n\
\n\
The Nightingale Operating System\n\
Version " NIGHTINGALE_VERSION "\n\
\n\
********************************\n\
\n\
Copyright (C) 2017-2022, Tyler Philbrick\n\
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

// move me
void fs_init(void *initfs);
void ext2_info(void);

// move me
void tty_init(void)
{
    struct tty *new_tty(struct serial_device * dev, int id);
    extern struct serial_device *x86_com[2];
    new_tty(x86_com[0], 0);
    new_tty(x86_com[1], 1);
}

void early_init(void)
{
    set_gs_base(cpus[0]);
    idt_install();

    heap_init(__global_heap_ptr, early_malloc_pool, EARLY_MALLOC_POOL_LEN);
    serial_init();

    // vmm_early_init(); // TODO fix first-start VMM

    tty_init();

    void limine_init(void);
    limine_init();

    pm_init();
    // pm_populate_with_limine_memmap();
}

__USED
noreturn void kernel_main(uint32_t mb_magic, uintptr_t mb_info)
{
    uint64_t tsc = rdtsc();

    early_init();

    for (;;) {
        asm volatile("hlt");
    }

    init_command_line();

    const char *boot_arg = get_kernel_argument("boot");
    if (boot_arg && strcmp(boot_arg, "quiet") == 0) {
        print_boot_info = false;
    }

    { // -> x86 arch_init()
        pic_init();
        // pic_irq_unmask(0); // Timer
        // pic_irq_unmask(4); // Serial
        // pic_irq_unmask(3); // Serial COM2

        if (supports_feature(_X86_FSGSBASE)) {
            enable_bits_cr4(1 << 16); // enable fsgsbase
            have_fsgsbase = 1;
        }
    }

    random_dance();
    event_log_init();
    timer_init();

    fs_init(initfs);

    threads_init();
    load_kernel_elf(mb_elf_tag());
    if (print_boot_info)
        pci_enumerate_bus_and_print();
    procfs_init();
    run_all_tests();

    initialized = true;

    /*
    acpi_rsdp_t *rsdp = limine_rsdp();
    acpi_init(rsdp);
    acpi_rsdt_t *rsdt = acpi_rsdt(NULL);
    acpi_madt_t *madt = acpi_get_table("APIC");
    */

    lapic_init();
    // ioapic_init(madt);

    const char *init_program = get_kernel_argument("init");
    if (!init_program)
        init_program = "/bin/init";
    bootstrap_usermode(init_program);

    if (0) {
        ext2_info();
    }

    printf("%s", banner);

    if (print_boot_info) {
        printf("threads: usermode thread installed\n");
        printf("initialization took: %li\n", rdtsc() - tsc);
        printf("cpu: allowing irqs\n");
    }
    enable_irqs();

    while (true)
        asm volatile("hlt");
    panic("kernel_main tried to return!");
}

void ap_kernel_main(void)
{
    printf("\nthis is the application processor\n");
    set_gs_base(cpus[1]);
    lapic_init();
}
