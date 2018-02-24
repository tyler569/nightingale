
#include <basic.h>
#include <string.h>
//#include <assert.h> // later, it's in panic for now
#include <multiboot2.h>
#include <debug.h>
#include <panic.h>
#include "cpu/vga.h"
#include "print.h"
#include "cpu/pic.h"
#include "cpu/pit.h"
// #include "cpu/portio.h"
#include <arch/x86/acpi.h>
#include <arch/x86/cpu.h>
#include "pmm.h"
#include "vmm.h"
#include "malloc.h"
#include "pci.h"
#include "kthread.h"
#include <vector.h>

void count_to_100() {
    for (int i=0; i<101; i++) {
        printf("%i ", i);
    }
    kthread_exit();
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


    apic_enable(0xFEE00000);// TMPTMP HARDCODE

    /*
    int timer_interval = 1000; // per second
    setup_interval_timer(timer_interval);
    printf("timer: running at %i/s\n", timer_interval);
    */

    uart_enable_interrupt(COM1);
    pic_irq_unmask(1); // Allow timer though
    printf("uart: listening for interrupts\n");

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

    vmm_map(0xfee00000, 0xfee00000);

    u32 *lapic_timer        = 0xfee00000 + 0x320;
    u32 *lapic_timer_count  = 0xfee00000 + 0x380;
    u32 *lapic_timer_divide = 0xfee00000 + 0x3E0;

    *lapic_timer_divide = 0x3;
    *lapic_timer_count = 250000;
    *lapic_timer = 0x20020;

    pci_enumerate_bus_and_print();


    printf("\n");
    printf("Project Nightingale\n");
    printf("\n");


// Network card driver testing
#ifdef __DOING_NETWORK_TESTING
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

// #define __DOING_MP

    extern usize timer_ticks;
    printf("This took %i ticks so far\n", timer_ticks);

// Multitasking
#ifdef __DOING_MP
    // kthread_create(test_kernel_thread);
    kthread_create(count_to_100);
    kthread_top();
    
    while (timer_ticks < 500) {
        // printf("*");
    }
#endif

    assert(false, "testing");

    panic("kernel_main tried to return!");
}

