
#include <ng/basic.h>
#include <ng/debug.h>
#include <ng/elf.h>
#include <ng/malloc.h>
#include <ng/multiboot.h>
#include <ng/multiboot2.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <ng/print.h>
#include <ng/rand.h>
#include <ng/string.h>
#include <ng/syscalls.h>
#include <ng/thread.h>
#include <ng/serial.h>
#include <ng/vmm.h>
#include <arch/x86/cpu.h>
#include <arch/x86/pic.h>
#include <arch/x86/pit.h>
// acpi testing
// #include <arch/x86/acpi.h>
// apic testing
// #include <arch/x86/apic.h>
#include <ds/list.h>
#include <fs/tarfs.h>
#include <ng/fs.h>
#include <net/network.h>

struct tar_header *initfs;

void test_kernel_thread() {
        enable_irqs(); // WHY IS THIS NEEDED ;-;

        printf("Hello World from a kernel thread\n");
        exit_kthread();
}

void kernel_main(uint32_t mb_magic, uintptr_t mb_info) {
        long tsc = rdtsc();
        mb_info += VMM_VIRTUAL_OFFSET;

        vmm_early_init();

        install_isrs();
        printf("idt: interrupts installed\n");

        pic_init();

        // TODO: BAD architecture specific things
        pic_irq_unmask(0); // Timer
        pic_irq_unmask(4); // Serial

        printf("pic: remapped and masked\n");
        printf("pit: running tickless\n");

        serial_init();
        printf("uart: ready\n");

        if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC)
                panic("Bootloader does not appear to be multiboot2.");
        mb_parse(mb_info);
        mb_mmap_print();

        size_t memory = mb_mmap_total_usable();
        size_t megabytes = memory / (1024 * 1024);
        size_t kilobytes = (memory - (megabytes * 1024 * 1024)) / 1024;
        printf("mmap: total usable memory: %zu (%zuMB + %zuKB)\n", memory,
               megabytes, kilobytes);

        printf("mb: kernel command line '%s'\n", mb_cmdline());

        initfs = (void *)mb_get_initfs();
        printf("mb: user init at %#zx\n", initfs);

        void *initfs_end = mb_get_initfs_end();
        // ensure the initfs is all mapped
        size_t initfs_len = (uintptr_t)initfs_end - (uintptr_t)initfs;
        //vmm_map_range((uintptr_t)initfs, (uintptr_t)initfs -
        //                VMM_VIRTUAL_OFFSET, initfs_len, 0);

        uintptr_t first_free_page = ((uintptr_t)initfs_end + 0x1fff) & ~0xfff;

        first_free_page -= VMM_VIRTUAL_OFFSET;

        printf("initfs at %#zx\n", initfs);
        printf("pmm: using %#zx as the first physical page\n", first_free_page);

        // So we have something working in the meantime
        pmm_allocator_init(first_free_page);

        kmalloc_global_region0 = vmm_reserve(128 * 1024*1024);
        malloc_initialize(kmalloc_global_region0, 128 * 1024*1024);

        init_global_lists();

        vfs_init();
        printf("vfs: filesystem initiated\n");

#if 0
        network_init();
        printf("network: network initialized\n");
#endif

        threads_init();
        printf("threads: process structures initialized\n");

        pci_enumerate_bus_and_print();

        printf("\n");
        printf("********************************\n");
        printf("\n");
        printf("The Nightingale Operating System\n");
        printf("Version " NIGHTINGALE_VERSION "\n");
        printf("\n");
        printf("********************************\n");
        printf("\n");

        enable_irqs();
        printf("cpu: allowing irqs\n");
        printf("initialization took: %li\n", rdtsc() - tsc);

        vfs_print_tree(fs_root_node, 0);

        new_kthread((uintptr_t)test_kernel_thread);
        bootstrap_usermode("/bin/init");

        while (true) {
                asm volatile("hlt");
        }

        panic("kernel_main tried to return!");
}

