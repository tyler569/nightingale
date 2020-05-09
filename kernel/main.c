
#include <basic.h>
#include <ng/debug.h>
#include <ng/fs.h>
#include <ng/malloc.h>
#include <ng/multiboot.h>
#include <ng/multiboot2.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <ng/print.h>
#include <ng/procfile.h>
#include <ng/rand.h>
#include <ng/string.h>
#include <ng/syscalls.h>
#include <ng/tarfs.h>
#include <ng/thread.h>
#include <ng/timer.h>
#include <ng/serial.h>
#include <ng/vmm.h>
// #include <ng/x86/acpi.h>
// #include <ng/x86/apic.h>
#include <ng/x86/cpu.h>
#include <ng/x86/pic.h>
#include <ng/x86/pit.h>
#include <ng/net/network.h>
#include <nc/list.h>
#include <nc/sys/time.h>
#include <linker/elf.h>

struct tar_header *initfs;

void test_kernel_thread() {
        printf("Hello World from a kernel thread\n");
        exit_kthread();
}

void proc_test(struct open_file *ofd) {
        ofd->buffer = malloc(KB);
        int count = sprintf(ofd->buffer, "This is a test procfile %x\n", 0x1234);
        ofd->length = count;
}

void kernel_main(uint32_t mb_magic, uintptr_t mb_info) {
        long tsc = rdtsc();

        heap_init(global_heap, early_malloc_pool, EARLY_MALLOC_POOL_LEN);

        heaptest();

        // vmm_early_init();
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

        size_t memory = mb_mmap_total_usable();
        size_t megabytes = memory / MB;
        size_t kilobytes = (memory - (megabytes * MB)) / KB;
        printf("mmap: total usable memory: %zu (%zuMB + %zuKB)\n", memory,
               megabytes, kilobytes);

        printf("mb: kernel command line '%s'\n", mb_cmdline());

        struct initfs_info initfs_info = mb_initfs_info();
        initfs = (struct tar_header *)initfs_info.base;
        uintptr_t initfs_end = initfs_info.end;
        size_t initfs_len = initfs_end - (uintptr_t)initfs;
        printf("mb: user init at %#zx\n", initfs);

        pm_mb_init(mb_mmap());
        pm_reserve((phys_addr_t)&_phy_kernel_base,
                   (phys_addr_t)&_phy_kernel_end, PM_KERNEL);

        pm_reserve(mb_phy_base(), mb_phy_end(), PM_MULTIBOOT);
        pm_reserve(mb_init_phy_base(), mb_init_phy_end(), PM_INITFS);

        pm_reserve((phys_addr_t)&_phy_kernel_base,
                   mb_init_phy_base(), PM_EVILHACK); // see below

        pm_alloc_page(); // dump the 0 page on the floor

        printf("pmm: physical memory map\n");
        pm_dump_regions();

        heaptest();

        printf("vmm: init kernel vmm\n");

        virt_addr_t kernel_base = (virt_addr_t)&_vm_kernel_start;
        virt_addr_t kernel_ro_end = (virt_addr_t)&_ro_end;
        size_t kernel_ro_len = kernel_ro_end - kernel_base;

        virt_addr_t kernel_rw_base = kernel_ro_end;
        virt_addr_t kernel_end = (virt_addr_t)&_kernel_end;

        virt_addr_t initfs_base = (virt_addr_t)initfs;

        // For some reason, it seems that my ELF parsing code is reading
        // addresses in the gap between the end of the kernel and the start
        // of the initfs.
        //
        // My theory is that that's implicitly where some of the ELF symbol
        // data ended up, but it's ugly that nothing seems to specify that.
        // The Multiboot information says the str and symtabs are inside the
        // multiboot info section.
        //
        // For now I'm going to hack around this by saying "the kernel ends
        // where init begins," but like, ugh. I should investigate this later.
        //
        // This is also changed above in the pm_reserve calls to prevent
        // allocating those physical pages.
        size_t kernel_rw_len = initfs_base - kernel_rw_base;

        struct kernel_mappings mappings[] = {
                { kernel_base, kernel_ro_len, PAGE_PRESENT },
                { kernel_rw_base, kernel_rw_len, PAGE_PRESENT | PAGE_WRITEABLE },
                { mb_base(), mb_length(), PAGE_PRESENT | PAGE_WRITEABLE },
                { initfs_base, initfs_len, PAGE_PRESENT | PAGE_WRITEABLE },
                { 0, 0, 0 },
        };

        heaptest();

        vm_kernel_init(mappings);
        vm_map_dump(vm_kernel);
        
        mb_elf_info(mb_elf_tag());
        init_timer_events();
        vfs_init(initfs_len);
        serial_ttys_init();
        threads_init();
        pci_enumerate_bus_and_print();
        procfs_init();

        printf("\n");
        printf("********************************\n");
        printf("\n");
        printf("The Nightingale Operating System\n");
        printf("Version " NIGHTINGALE_VERSION "\n");
        printf("\n");
        printf("********************************\n");
        printf("\n");

        // vfs_print_tree(fs_root_node, 0);

        timer_enable_periodic(HZ);

        new_kthread((uintptr_t)test_kernel_thread);

        struct process *init = bootstrap_usermode("/bin/init");
        printf("threads: usermode thread installed\n");

        printf("initialization took: %li\n", rdtsc() - tsc);

        printf("cpu: allowing irqs\n");
        enable_irqs();

        while (true) {
                asm volatile("hlt");
        }

        panic("kernel_main tried to return!");
}

