
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
#include <arch/x86/vga.h>
#include <arch/x86/pic.h>
#include <arch/x86/pit.h>
#include <arch/x86/acpi.h>
#include <arch/x86/cpu.h>

void count_to_100() {
    for (int i=0; i<101; i++) {
        printf("%i ", i);
        asm volatile ("hlt"); // or hlt
    }
    exit_kthread();
}

void kernel_main(u32 mb_magic, usize mb_info) {

// initialization
    vga_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    vga_clear();
    uart_init(COM1);
    printf("terminal: initialized\n");
    printf("uart: initialized\n");

    pic_init(); // leaves everything masked
    // pic_irq_unmask(0); // Allow timer though
    printf("pic: remapped and masked\n");



    /*
    int timer_interval = 1000; // per second
    setup_interval_timer(timer_interval);
    printf("timer: running at %i/s\n", timer_interval);
    */

    // uart_enable_interrupt(COM1);
    // pic_irq_unmask(1); // Allow timer though
    // printf("uart: listening for interrupts\n");

    enable_irqs();
    printf("cpu: allowing irqs\n");

    if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        panic("Bootloader does not appear to be multiboot2.");
    mb_parse(mb_info);
    mb_mmap_print();

    usize memory = mb_mmap_total_usable();
    printf("mmap: total usable memory: %lu (%luMB)\n", memory, memory / (1024 * 1024));

    usize size = *(u32 *)mb_info;
    if (size + mb_info >= 0x1c0000)
        panic("Multiboot data structure overlaps hard-coded start of heap!");

    // pretty dirty thing - just saying "memory starts after the multiboot header"...
    // TODO: Cleanup
    usize first_free_page = (size + mb_info + 0xfff) & ~0xfff;

    // usize last_free_page = 0;
    // last_free_page = mb_mmap_end_of_section_with(first_free_page)
    // pmm_allocator_init(first_free_page, last_free_page);
    // acpi_init(mb_rsdp)
    // acpi_find_table(MADT)

    // So we have something working in the meantime
    pmm_allocator_init(first_free_page, 0x2000000); // TEMPTEMPTEMPTEMP

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
    // backtrace(3);

    acpi_rsdp *rsdp = mb_acpi_get_rsdp();
    acpi_rsdt *rsdt = acpi_get_rsdt(rsdp);
    vmm_map(rsdt, rsdt);
    printf("acpi: RSDT found at %lx\n", rsdt);
    acpi_madt *madt = acpi_get_table(MADT);
    if (!madt)  panic("No MADT found!");
    printf("acpi: MADT found at %lx\n", madt);
    acpi_print_table(madt);

    enable_apic(0xFEE00000);// TMPTMP HARDCODE

    u32 *lapic_timer        = 0xfee00000 + 0x320;
    u32 *lapic_timer_count  = 0xfee00000 + 0x380;
    u32 *lapic_timer_divide = 0xfee00000 + 0x3E0;

    *lapic_timer_divide = 0x3;      // divide by 16
    *lapic_timer_count = 100000;    // initial countdown amount
    *lapic_timer = 0x20020;         // enabled, periodic, not masked

    pci_enumerate_bus_and_print();


    printf("\n");
    printf("Project Nightingale\n");
    printf("\n");


// Network card driver testing
#ifdef __TEST_NIC
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

    vmm_map(base, base);
    printf("%#010x\n", *(u32 *)base);
    printf("%#010x\n", *(u32 *)(base + 0x08));
    printf("%#010x\n", *(u32 *)(base + 0x10));
#endif

    extern volatile usize timer_ticks;
    printf("This took %i ticks so far\n", timer_ticks);

#define __TEST_MP
#ifdef __TEST_MP
    create_kthread(test_kernel_thread);
    create_kthread(count_to_100);
    create_kthread(count_to_100);
    create_kthread(count_to_100);
    create_kthread(count_to_100);
    kthread_top();
#endif

#ifdef __TEST_VMM_FAIL
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

    enable_irqs();
    while (timer_ticks < 10) {
        printf("*");
        // asm volatile ("pause");
    }

    // *(volatile int *)0x5123213213 = 100; // testing backtrace

    panic("kernel_main tried to return!");
}

