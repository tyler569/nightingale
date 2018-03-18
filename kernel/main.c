
#include <basic.h>
#include <string.h>
//#include <assert.h> // later, it's in panic for now
#include <multiboot2.h>
#include <debug.h>
#include <panic.h>
#include "print.h"
#include "pmm.h"
#include "vmm.h"
#include "multiboot.h"
#include "malloc.h"
#include "pci.h"
#include "kthread.h"
#include "vector.h"
#include <mutex.h>
#include <arch/x86/vga.h>
#include <arch/x86/pic.h>
#include <arch/x86/pit.h>
#include <arch/x86/acpi.h>
#include <arch/x86/apic.h>
#include <arch/x86/cpu.h>

extern kthread_t *current_kthread;

void test_thread() {
    for (int j=0; j<10000; j++) {}
    
    int foo;
    printf("thread %i @ %#x\n", current_kthread->id, &foo);

    exit_kthread();
}

void test_user_thread() {
    int a = 10;
    int b = 10;
    a += b;
    b += a;
    asm volatile ("int $128");
    while (true);
}

static kmutex test_mutex = KMUTEX_INIT;

void kernel_main(uint32_t mb_magic, uintptr_t mb_info) {

// initialization
    vga_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    vga_clear();
    uart_init(COM1);
    printf("terminal: initialized\n");
    printf("uart: initialized\n");

    install_isrs();
    pic_init(); // leaves everything masked
    pic_irq_unmask(0); // Allow timer though
    printf("pic: remapped and masked\n");

    int timer_interval = 100; // per second
    set_timer_periodic(timer_interval);
    printf("timer: running at %i/s\n", timer_interval);

    uart_enable_interrupt(COM1);
    pic_irq_unmask(1); // Allow timer though // keyboard? serial?
    printf("uart: listening for interrupts\n");

    enable_irqs();
    printf("cpu: allowing irqs\n");

    if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        panic("Bootloader does not appear to be multiboot2.");
    mb_parse(mb_info);
    mb_mmap_print();

    size_t memory = mb_mmap_total_usable();
    printf("mmap: total usable memory: %lu (%luMB)\n", memory, memory / (1024 * 1024));

    size_t size = *(uint32_t *)mb_info;
    if (size + mb_info >= 0x1c0000)
        panic("Multiboot data structure overlaps hard-coded start of heap!");

    // pretty dirty thing - just saying "memory starts after the multiboot header"...
    // TODO: Cleanup
    uintptr_t first_free_page = (size + mb_info + 0xfff) & ~0xfff;

    // So we have something working in the meantime
    pmm_allocator_init(first_free_page, 0x2000000); // TEMPTEMPTEMPTEMP

#if 0 // ELF parsing is on back burner
    mb_elf_print();
    void *elf = mb_elf_get();
    printf("elf: \n");
    for (int i=0; i<18; i++) {
        if (((u32 *)(elf + 64*i))[1] == 3)
            // debug_dump(((u64 *)(elf + 64*i))[2]);
        printf(" section type %u at %#lx\n", 
                ((u32 *)(elf + 64*i))[1],
                ((u64 *)(elf + 64*i))[2]);
    }
#endif

#if 0 // ACPI is on back burner
    acpi_rsdp *rsdp = mb_acpi_get_rsdp();
    acpi_rsdt *rsdt = acpi_get_rsdt(rsdp);
    vmm_map((uintptr_t)rsdt, (uintptr_t)rsdt);
    printf("acpi: RSDT found at %lx\n", rsdt);
    acpi_madt *madt = acpi_get_table(MADT);
    if (!madt)  panic("No MADT found!");
    printf("acpi: MADT found at %lx\n", madt);
    acpi_print_table((acpi_header *)madt);
#endif

#if 0 // APIC is on back burner
    enable_apic(0xFEE00000);// TMPTMP HARDCODE

    volatile uint32_t *lapic_timer        = (volatile uint32_t *)(0xfee00000 + 0x320);
    volatile uint32_t *lapic_timer_count  = (volatile uint32_t *)(0xfee00000 + 0x380);
    volatile uint32_t *lapic_timer_divide = (volatile uint32_t *)(0xfee00000 + 0x3E0);

    *lapic_timer_divide = 0x3;      // divide by 16
    *lapic_timer_count = 50000;     // initial countdown amount
    *lapic_timer = 0x20020;         // enabled, periodic, not masked
#endif

// Network card driver testing
#if 0 // Net is on back burner
    u32 network_card = pci_find_device_by_id(0x8086, 0x100e);
    printf("Network card ID = ");
    pci_print_addr(network_card);
    printf("\n");
    printf("BAR0: %#010x\n", pci_config_read(network_card + 0x10));
    u32 base = pci_config_read(network_card + 0x10);
    printf("BAR1: %#010x\n", pci_config_read(network_card + 0x14));
    printf("BAR2: %#010x\n", pci_config_read(network_card + 0x18));
    printf("BAR3: %#010x\n", pci_config_read(network_card + 0x1c));
    printf("BAR4: %#010x\n", pci_config_read(network_card + 0x20));
    printf("BAR5: %#010x\n", pci_config_read(network_card + 0x24));

    vmm_map((uintptr_t)base, (uintptr_t)base);
    printf("%#010x\n", *(u32 *)base);
    printf("%#010x\n", *(u32 *)(base + 0x08));
    printf("%#010x\n", *(u32 *)(base + 0x10));
#endif

#if 0 // VMM fail - save for testing infra
    uintptr_t page = pmm_allocate_page();
    uintptr_t vma = 0x450000;
    vmm_map(vma, page);
    int *value = (int *)vma;

    *value = 100;
    printf("before: %i\n", *value);
    vmm_edit_flags(vma, 0); // !WRITEABLE
    printf("after read : %i\n", *value);
    *value = 100;
    printf("after write: %i\n", *value);
#endif

#ifdef __TEST_UBSAN
    int bar = (int)0x7FFFFFFF;
    bar += 1;
    printf("%i\n", bar);
#endif

    pci_enumerate_bus_and_print();

    printf("\n");
    printf("Project Nightingale\n");
    printf("\n");

    printf("try acquire test mutex: %i\n", try_acquire_mutex(&test_mutex));
    printf("try acquire test mutex: %i\n", try_acquire_mutex(&test_mutex));
    printf("release test mutex: %i\n", release_mutex(&test_mutex));

    printf("acquire test mutex: %i\n", await_mutex(&test_mutex));
    printf("try acquire test mutex: %i\n", try_acquire_mutex(&test_mutex));
    printf("release test mutex: %i\n", release_mutex(&test_mutex));

    printf("acquire test mutex: %i\n", await_mutex(&test_mutex));

    extern volatile usize timer_ticks;
    printf("This took %i ticks so far\n", timer_ticks);

    create_kthread(thread_watchdog);
    create_kthread(test_thread);
    create_user_thread(test_user_thread);

    /*
    for (int x=0; x<5; x++) {
        create_kthread(count_to_100);
    }
    */

    kthread_top();

    
    while (true) {
        // printf("*");
        asm volatile ("pause");
    }

    kthread_top();

    printf("That took %i ticks\n", timer_ticks);

    // *(volatile int *)0x5123213213 = 100; // testing backtrace

    panic("kernel_main tried to return!");
}

