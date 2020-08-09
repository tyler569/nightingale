
#include <basic.h>
#include <ng/debug.h>
#include <ng/fs.h>
#include <ng/multiboot.h>
#include <ng/multiboot2.h>
#include <ng/mutex.h>
#include <ng/panic.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <ng/procfile.h>
#include <ng/rand.h>
#include <ng/spalloc.h>
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
#include <linker/elf.h>
#include <sys/time.h>
#include <list.h>
#include <stdlib.h>
#include <stdio.h>

struct tar_header *initfs;

void test_kernel_thread(void *arg) {
        printf("Hello World from a kernel thread\n");
        const char *message = arg;
        printf("The message is '%s'!\n", message);

        kthread_exit();
}

void proc_test(struct open_file *ofd) {
        ofd->buffer = malloc(KB);
        int count = sprintf(ofd->buffer, "This is a test procfile %x\n", 0x1234);
        ofd->length = count;
}

void kernel_main(uint32_t mb_magic, uintptr_t mb_info) {
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

        // TODO: jank bad way to find available memory
        {
        uintptr_t first_free_page = ((uintptr_t)initfs_end + 0x1fff) & ~0xfff;
        first_free_page -= VMM_VIRTUAL_OFFSET;
        printf("initfs at %#zx\n", initfs);
        printf("pmm: using %#zx as the first physical page\n", first_free_page);
        pmm_allocator_init(first_free_page);
        }

        mb_elf_info(mb_elf_tag());
        init_timer();
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

        printf("Copyright (C) 2017-2020, Tyler Philbrick\n");
        printf("This program comes with ABSOLUTELY NO WARRANTY\n");
        printf("Nightingale is free software, and you are welcome to redistribute it under the terms\n");
        printf("of the the GNU General Public License as published by the Free Software Foundation\n");
        printf("You should have received a copy of the GNU General Public License\n");
        printf("along with this program. If not, see <https://www.gnu.org/licenses/>.\n");
        printf("\n");

        // vfs_print_tree(fs_root_node, 0);

        timer_enable_periodic(HZ);

        kthread_create(test_kernel_thread, "get a cat");

        struct process *init = bootstrap_usermode("/bin/init");
        printf("threads: usermode thread installed\n");

        printf("initialization took: %li\n", rdtsc() - tsc);

        printf("cpu: allowing irqs\n");
        enable_irqs();

        {
                // validate spalloc working
                struct testing {
                        int a, b, c, d, e, f, g, h;
                };
                struct spalloc foobar;
                sp_init(&foobar, struct testing);

                struct testing *first = sp_alloc(&foobar);
                assert(first == foobar.region);
                first->a = 10;

                struct testing *second = sp_alloc(&foobar);
                assert(second == sp_at(&foobar, 1));
                second->a = 11;

                assert(first->a == 10);

                first->g = 1;
                sp_free(&foobar, first);
                assert(first->g != 1); // poison
                assert(second->a == 11);

                struct testing *re_first = sp_alloc(&foobar);
                assert(re_first == first);

                assert(foobar.capacity == 0x1000);
                assert(foobar.count == 2);
        }

        printf("Hello World\n");
        sleep_thread(10000);
        printf("Slept world\n");

        while (true) {
                asm volatile("hlt");
        }

        panic("kernel_main tried to return!");
}

