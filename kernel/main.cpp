#include <elf.h>
#include <ng/arch.h>
#include <ng/commandline.h>
#include <ng/drv/nic_rtl8139.h>
#include <ng/drv/pci.h>
#include <ng/event_log.h>
#include <ng/fs/init.h>
#include <ng/irq.h>
#include <ng/limine.h>
#include <ng/mt/process.h>
#include <ng/panic.h>
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
#include <nx/print.h>
#include <stdio.h>
#include <stdlib.h>
#include <version.h>

tar_header *initfs;
bool have_fsgsbase = false;
bool initialized = false;

const char *banner = "\n\
********************************\n\
\n\
The Nightingale Operating System\n\
Version " NIGHTINGALE_VERSION "\n\
\n\
********************************\n\
\n\
Copyright (C) 2017-2023, Tyler Philbrick\n";

bool print_boot_info = true;

[[noreturn]] void real_main();
extern char hhstack_top;

extern "C" {
void (*ctors_begin)();
void (*ctors_end)();
}

class foo {
public:
    foo() { nx::print("ctors: working\n"); }
};

foo f {};

void call_global_constructors()
{
    for (const auto *ctor = &ctors_begin; ctor != &ctors_end; ctor++) {
        // nx::print("ctor found: %\n", *ctor);
        (*ctor)();
    }
}

extern "C" void early_init(void)
{
    set_gs_base(thread_cpus[0]);
    idt_install();
    heap_init(__global_heap_ptr, early_malloc_pool, EARLY_MALLOC_POOL_LEN);
    serial_init();
    arch_init();
    acpi_init(static_cast<acpi_rsdp_t *>(limine_rsdp()));
    tty_init();
    pm_init();
    limine_init();
    call_global_constructors();
}

uint64_t tsc;

extern "C" [[noreturn, maybe_unused]] void kernel_main(void)
{
    tsc = rdtsc();
    early_init();
    random_dance();
    event_log_init();
    timer_init();
    longjump_kcode((uintptr_t)real_main, (uintptr_t)&hhstack_top);
}

void start_networking()
{
    if (auto addr = pci_find_device(0x10ec, 0x8139); addr) {
        nx::print("rtl8139: at %\n", *addr);
        auto rtl8139 = new nic_rtl8139(*addr);
        int irq = rtl8139->interrupt_number();
        nx::print("rtl8139: irq %\n", irq);

        irq_install(irq, [=](auto *) { rtl8139->interrupt_handler(); });

        unsigned char ethernet_frame[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0x52, 0x54, 0x00, 0x12, 0x34, 0x56, 0x08, 0x06, 0x00, 0x01, 0x08,
            0x00, 0x06, 0x04, 0x00, 0x01, 0x52, 0x54, 0x00, 0x12, 0x34, 0x56,
            0x0a, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a,
            0x00, 0x02, 0x02 };
        rtl8139->send_packet(ethernet_frame, sizeof(ethernet_frame));
        rtl8139->send_packet(ethernet_frame, sizeof(ethernet_frame));
    } else {
        nx::print("rtl8139: not found\n");
    }
}

[[noreturn]] void ap_real_main()
{
    printf("\nthis is the application processor\n");
    arch_ap_init();
    printf("lapic: initialized\n");

    while (true)
        __asm__ volatile("hlt");
}

[[noreturn]] void real_main()
{
    nx::print("*** real_main ***\n");

    void *kernel_file_ptr = limine_kernel_file_ptr();
    size_t kernel_file_len = limine_kernel_file_len();
    limine_load_kernel_elf(kernel_file_ptr, kernel_file_len);

    initfs = static_cast<tar_header *>(limine_module());
    fs_init(initfs);
    threads_init();

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
        printf("initialization took: %li\n", rdtsc() - tsc);
        printf("cpu: allowing irqs\n");
    }

    enable_irqs();
    start_networking();

    limine_smp_init(1, reinterpret_cast<limine_goto_address>(ap_real_main));

    while (true)
        __asm__ volatile("hlt");
    panic("kernel_main tried to return!");
}

extern "C" [[maybe_unused]] void __cxa_atexit()
{
    // it is impossible to exit the kernel without a panic, so running
    // global destructors is never needed and can be a no-op.
}