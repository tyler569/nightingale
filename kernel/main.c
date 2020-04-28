
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

extern char _phy_kernel_base;
extern char _phy_kernel_end;

void heaptest() {
#define rounds 100
#define x 100
#define w 100
        char *foo[x] = {0};

        for (int k=0; k<rounds; k++) {
                for (int i=0; i<x; i++)  foo[i] = zmalloc(w);
                for (int i=0; i<x; i++) 
                        for (int j=0; j<w; j++)  assert(foo[i][j] == 0);
                for (int i=0; i<x; i++)  free(foo[i]);
        }

#undef rounds
#undef x
#undef w
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

        printf("pmm: physical memory map\n");
        pm_dump_regions();

        heaptest();

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

        int err = bootstrap_usermode("/bin/init");
        if (err < 0) {
                panic("error bootstrapping usermode");
        }
        struct process *init = process_by_id(1);
        struct thread *init_thread = list_head_entry(
                        struct thread, &init->threads, process_threads);
        enqueue_thread_at_front(init_thread);
        printf("threads: usermode thread installed\n");

        printf("initialization took: %li\n", rdtsc() - tsc);

        printf("cpu: allowing irqs\n");
        enable_irqs();

        while (true) {
                asm volatile("hlt");
        }

        panic("kernel_main tried to return!");
}

