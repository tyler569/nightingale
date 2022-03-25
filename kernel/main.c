#include <basic.h>
#include <ng/commandline.h>
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
#include <x86/cpu.h>
#include <x86/lapic.h>
#include <x86/pic.h>

struct tar_header *initfs;
int have_fsgsbase = 0;

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
    proc2_sprintf(ofd, "Hello World\n");
}

void procfs_init()
{
    extern void timer_procfile(struct file *, void *);
    extern void proc_syscalls(struct file *, void *);
    extern void proc_mods(struct file *, void *);
    extern void pm_summary(struct file *, void *);
    extern void proc_heap(struct file * file, void *_);

    make_proc_file2("test", proc_test, NULL);
    make_proc_file2("timer", timer_procfile, NULL);
    make_proc_file2("mem", pm_summary, NULL);
    make_proc_file2("syscalls", proc_syscalls, NULL);
    make_proc_file2("mods", proc_mods, NULL);
    make_proc_file2("heap", proc_heap, NULL);

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

__USED noreturn void kernel_main(uint32_t mb_magic, uintptr_t mb_info)
{
    uint64_t tsc = rdtsc();
    phys_addr_t kernel_base = (phys_addr_t)&_kernel_phy_base;
    phys_addr_t kernel_top = (phys_addr_t)&_kernel_phy_top;

    // panic_bt doesn't work until after the IDT is installed
    idt_install();

    set_gs_base(&thread_zero);

    // serial_init needs the heap to be initialized first
    heap_init(__global_heap_ptr, early_malloc_pool, EARLY_MALLOC_POOL_LEN);
    serial_init();
    vmm_early_init();

    // FIXME EWWW
    struct serial_device;
    struct tty *new_tty(struct serial_device * dev, int id);
    extern struct serial_device *x86_com[2];
    new_tty(x86_com[0], 0);
    new_tty(x86_com[1], 1);

    // update page table mappings in boot.S if thiis fails
    assert(kernel_top < 0x200000);

    pm_init();
    pm_set(0, 0x1000, PM_LEAK);
    pm_set(kernel_base, kernel_top, PM_LEAK);

    if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        panic("Bootloader does not appear to be multiboot2.");

    mb_init(mb_info);
    mb_mmap_enumerate(mb_pm_callback);

    init_command_line();

    const char *boot_arg = get_kernel_argument("boot");
    if (boot_arg && strcmp(boot_arg, "quiet") == 0) {
        print_boot_info = false;
    }

    if (print_boot_info)
        mb_mmap_print();

    { // -> x86 arch_init()
        pic_init();
        pic_irq_unmask(0); // Timer
        pic_irq_unmask(4); // Serial
        pic_irq_unmask(3); // Serial COM2

        if (supports_feature(_X86_FSGSBASE)) {
            enable_bits_cr4(1 << 16); // enable fsgsbase
            have_fsgsbase = 1;
        }
    }

    size_t memory = mb_mmap_total_usable();
    size_t megabytes = memory / MB;
    size_t kilobytes = (memory - (megabytes * MB)) / KB;
    if (print_boot_info) {
        printf("mmap: total usable memory:");
        printf("%zu (%zuMB + %zuKB)\n", memory, megabytes, kilobytes);
        printf("mb: kernel command line '%s'\n", mb_cmdline());
        printf("mb: bootloader is '%s'\n", mb_bootloader());
    }

    if (print_boot_info)
        printf("kernel: %10zx - %10zx\n", kernel_base, kernel_top);

    struct initfs_info initfs_info = mb_initfs_info();
    initfs = (struct tar_header *)(initfs_info.base + VMM_KERNEL_BASE);
    if (print_boot_info)
        printf("mb: user init at %p - %#lx\n", (void *)initfs, initfs_info.top);
    pm_set(initfs_info.base, initfs_info.top, PM_LEAK);

    // FIXME: the elf metadata ends up here, outside of the end of the
    // file for some reason.
    pm_set(kernel_top, initfs_info.base, PM_LEAK);

    size_t initfs_len = initfs_info.top - initfs_info.base;
    uintptr_t initfs_v = initfs_info.base + VMM_KERNEL_BASE;
    uintptr_t initfs_p = initfs_info.base;

    vmm_unmap_range(initfs_v, initfs_len);
    vmm_map_range(initfs_v, initfs_p, initfs_len, 0); // init is read-only

    random_dance();
    event_log_init();
    timer_init();
    // vfs_init(initfs_info.top - initfs_info.base);

    void fs2_init(void *initfs);
    fs2_init(initfs);

    threads_init();
    load_kernel_elf(mb_elf_tag());
    if (print_boot_info)
        pci_enumerate_bus_and_print();
    procfs_init();
    run_all_tests();

    if (0) {
        acpi_rsdp_t *rsdp = mb_acpi_rsdp();
        acpi_print_rsdp(rsdp);
        acpi_init(rsdp);
        acpi_rsdt_t *rsdt = acpi_rsdt(NULL);
        acpi_print_table(&rsdt->header);
        acpi_madt_t *madt = acpi_get_table("APIC");
        if (madt)
            acpi_print_table((void *)madt);
        else
            printf("NO APIC TABLE!!!\n");
        printf("this is cpu %i\n", cpunum());
    }

    if (0) {
        extern char ap_trampoline;
        vmm_map(0x8000, 0x8000, PAGE_WRITEABLE);
        memcpy((void *)0x8000, &ap_trampoline, 0x1000);

        lapic_init();
        lapic_send_init(1);
        delay(10000);
        lapic_send_ipi(IPI_SIPI, 0x08, 1);
    }

    const char *init_program = get_kernel_argument("init");
    if (!init_program)
        init_program = "/bin/init";
    bootstrap_usermode(init_program);

    printf("%s", banner);
    timer_enable_periodic(HZ);

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
