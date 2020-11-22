#include <basic.h>
#include <linker/elf.h>
#include <list.h>
#include <ng/debug.h>
#include <ng/fs.h>
#include <ng/multiboot.h>
#include <ng/multiboot2.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <ng/rand.h>
#include <ng/serial.h>
#include <ng/spalloc.h>
#include <ng/string.h>
#include <ng/syscalls.h>
#include <ng/tarfs.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/vmm.h>
#include <ng/x86/cpu.h>
#include <ng/x86/pic.h>
#include <ng/x86/pit.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

struct tar_header *initfs;

void test_kernel_thread(void *arg) {
    printf("Hello World from a kernel thread\n");
    const char *message = arg;
    printf("The message is '%s'!\n", message);

    kthread_exit();
}

void lots_of_threads(void *message) {
    printf("%s", message);
    kthread_exit();
}

noreturn void test_sleepy_thread(void *_) {
    while (true) {
        printf("sleepy thread");
        sleep_thread(seconds(1));
    }
}

sysret sys_syscall_test(char *buffer) {
    strcpy(buffer, "pizza");
    pm_summary();
    return 0;
}

void mb_pm_callback(phys_addr_t mem, size_t len, int type) {
    int pm_type;
    if (type == MULTIBOOT_MEMORY_AVAILABLE) {
        pm_type = PM_REF_ZERO;
    } else {
        pm_type = PM_LEAK;
    }
    pm_set(mem, mem + len, pm_type);
}

extern char _kernel_phy_base;
extern char _kernel_phy_top;

noreturn void kernel_main(uint32_t mb_magic, uintptr_t mb_info) {
    long tsc = rdtsc();

    heap_init(global_heap, early_malloc_pool, EARLY_MALLOC_POOL_LEN);

    vmm_early_init();
    install_isrs();
    pic_init();

    // TODO: BAD architecture specific things
    pic_irq_unmask(0); // Timer
    pic_irq_unmask(4); // Serial
    pic_irq_unmask(3); // Serial COM2

    serial_init();

    if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        panic("Bootloader does not appear to be multiboot2.");
    mb_init(mb_info);
    mb_mmap_print();
    mb_mmap_enumerate(mb_pm_callback);

    phys_addr_t kernel_base = (phys_addr_t)&_kernel_phy_base;
    phys_addr_t kernel_top = (phys_addr_t)&_kernel_phy_top;
    pm_set(kernel_base, kernel_top, PM_LEAK);

    size_t memory = mb_mmap_total_usable();
    size_t megabytes = memory / MB;
    size_t kilobytes = (memory - (megabytes * MB)) / KB;
    printf("mmap: total usable memory: %zu (%zuMB + %zuKB)\n", memory,
           megabytes, kilobytes);
    printf("mb: kernel command line '%s'\n", mb_cmdline());

    struct initfs_info initfs_info = mb_initfs_info();
    initfs = (struct tar_header *)(initfs_info.base + VMM_KERNEL_BASE);
    printf("mb: user init at %#zx - %#zx\n", initfs, initfs_info.top);
    pm_set(initfs_info.base, initfs_info.top, PM_LEAK);

    // FIXME: the elf metadata ends up here, outside of the end of the
    // file for some reason.
    pm_set(kernel_top, initfs_info.base, PM_LEAK);

    pm_summary();

    mb_elf_info(mb_elf_tag());
    init_timer();
    vfs_init(initfs_info.top - initfs_info.base);
    threads_init();
    pci_enumerate_bus_and_print();

    printf("\n");
    printf("********************************\n");
    printf("\n");
    printf("The Nightingale Operating System\n");
    printf("Version " NIGHTINGALE_VERSION "\n");
    printf("\n");
    printf("********************************\n");
    printf("\n");

    printf("Copyright (C) 2017-2020, Tyler Philbrick\n");
    printf("This program comes with ABSOLUTELY NO WARRANTY\n");
    printf("Nightingale is free software, and you are"
           "welcome to redistribute it under the terms\n");
    printf("of the the GNU General Public License as published"
           "by the Free Software Foundation\n");
    printf(
        "You should have received a copy of the GNU General Public License\n");
    printf("along with this program. If not, see "
           "<https://www.gnu.org/licenses/>.\n");
    printf("\n");

    timer_enable_periodic(HZ);

    kthread_create(test_kernel_thread, "get a cat");

    for (int i = 0; i < 5; i++) {
        kthread_create(lots_of_threads, "a");
        kthread_create(lots_of_threads, "b");
    }

    struct process *init = bootstrap_usermode("/bin/init");
    printf("threads: usermode thread installed\n");

    printf("initialization took: %li\n", rdtsc() - tsc);

    printf("cpu: allowing irqs\n");
    enable_irqs();

    void run_all_tests(void);
    // run_all_tests();

    while (true) { asm volatile("hlt"); }

    panic("kernel_main tried to return!");
}
