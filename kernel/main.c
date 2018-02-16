
#include <string.h>

#include <basic.h>
//#include <assert.h> // later, it's in panic for now
#include <multiboot2.h>
#include <debug.h>
#include <panic.h>
#include "cpu/vga.h"
#include "print.h"
#include "cpu/pic.h"
#include "cpu/pit.h"
#include "cpu/portio.h"
#include "pmm.h"
#include "vmm.h"
#include "malloc.h"
#include "pci.h"
#include "proc.h"
#include <vector.h>

void count_to_100() {
    for (int i=0; i<101; i++) {
        printf("%i ", i);
    }
    proc_exit();
}

void kernel_main(u32 mb_magic, usize mb_info) {

// initialization
    vga_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    vga_clear();
    uart_init(COM1);
    printf("Terminal Initialized\n");
    printf("UART Initialized\n");

    remap_pic();
    for (int i=0; i<16; i++) {
        if (i == 0) continue; // I need timer
        mask_irq(i);
    }
    printf("PIC remapped\n");

    setup_interval_timer(1000);
    printf("Interval Timer Initialized\n");

    uart_enable_interrupt(COM1);
    printf("Serial Interrupts Initialized\n");

    enable_irqs();
    printf("IRQs Enabled\n");

// testing length of kernel
    extern usize _kernel_start;
    extern usize _kernel_end;

    usize len = (usize)&_kernel_end - (usize)&_kernel_start;

    // Why tf does _kernel_start = .; not work in link.ld?
    if ((usize)&_kernel_start == 0x100000) {
        printf("_kernel_start = %p;\n", &_kernel_start);
    } else {
        printf("_kernel_start = %p; // wtf?\n", &_kernel_start);
    }
    printf("_kernel_end   = %p;\n", &_kernel_end);
    printf("\n");
    printf("kernel is %i kilobytes long\n", len / 1024);
    printf("kernel is %x bytes long\n", len);
    printf("\n");

// Multiboot
    printf("Multiboot magic: %#x\n", mb_magic);
    printf("Multiboot info*: %p\n", mb_info);

    assert(mb_magic == MULTIBOOT2_BOOTLOADER_MAGIC,
           "Hair on fire, bootloader must be multiboot2");

    multiboot_tag *tag;

    usize size = *(u32 *)mb_info;
    printf("Multiboot announced size %i\n\n", size);
    printf("Which makes the end %p\n", size + mb_info);

    assert(size + mb_info < 0x1c0000,
           "The heap is hard-coded to start at 0x1c0000.  We ran out of space.");

    usize first_free_page = (size + mb_info + 0xfff) & ~0xfff;
    usize last_free_page = 0;
    printf("first_free_page = %p\n", first_free_page);

    printf("\n");

    for (tag = (multiboot_tag *)(mb_info+8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (multiboot_tag *)((u8 *)tag + ((tag->size+7) & ~7))) {

        printf("tag type: %i, size: %i\n", tag->type, tag->size);
        switch (tag->type) {
        case MULTIBOOT_TAG_TYPE_CMDLINE: {
            multiboot_tag_string *cmd_line_tag = (void *)tag;
            printf("Command line = \"%s\"\n", &cmd_line_tag->string);
            // parse_command_line(&cmd_line_tag->string);
            break;
        }
        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: {
            printf ("boot loader name = \"%s\"\n",
                   ((struct multiboot_tag_string *) tag)->string);
            break;
        }
        case MULTIBOOT_TAG_TYPE_MMAP: {
            multiboot_mmap_entry *mmap;

            printf("Memory map:\n");

            for (mmap = ((multiboot_tag_mmap *)tag)->entries;
                 (u8 *)mmap < (u8 *)tag + tag->size;
                 mmap = (multiboot_mmap_entry *)((unsigned long) mmap
                     + ((struct multiboot_tag_mmap *) tag)->entry_size)) {

                printf("base: %p, len: %x (%iM), type %i\n",
                        mmap->addr, mmap->len, mmap->len/(1024*1024), mmap->type);
                // HACK to find the real memory
                if (last_free_page == 0 && first_free_page > mmap->addr &&
                    first_free_page < (mmap->addr + mmap->len) && mmap->type == 1) {

                    last_free_page = mmap->addr + mmap->len - 0x1000;
                }
            }
            break;
        }
        case MULTIBOOT_TAG_TYPE_ELF_SECTIONS: {
            multiboot_tag_elf_sections *elf = (void *)tag;
            printf("ELF Sections:\n");
            printf("size    = %i\n", tag->size);
            printf("num     = %i\n", elf->num);
            printf("entsize = %i\n", elf->entsize);
            //panic();
            break;
        }
        default:
            printf("unhandled\n");
        }
    }
    printf("\n");

// pmm setup

    pmm_allocator_init(first_free_page, last_free_page);
    printf("Setup physical allocator: %p -> %p\n", first_free_page, last_free_page);
    printf("Allocate vmm test: %p\n", pmm_allocate_page());

    printf("\n");

// vmm resolution and mapping
    usize resolved1 = vmm_virt_to_phy(0x101888);
    printf("resolved vma:%p to pma:%p\n", 0x101888, resolved1);

    usize *test_vmm_allocator = (usize *)0x123456789000;
    vmm_map((usize)test_vmm_allocator, pmm_allocate_page());
    *test_vmm_allocator = 100;
    printf("*%p = %i\n", test_vmm_allocator, *test_vmm_allocator);

    printf("\n");

    // debug_dump(pointer_to_be);'

// test malloc

#if 0
    for (usize i=0; i<100; i++) {
        int *p = malloc(0x10000);
        p[0] = 10;
        if (p[0] != 10) {
            panic("Memory backed badly at %p\n");
        }
    }
    printf("Malloc test got to %p without error\n", malloc(1));

    printf("\n");
#endif

// u128 test
    u128 x128_test = 0;
    x128_test -= 1;
    printf("Debug dump system, and u128(-1):\n");
    debug_dump(&x128_test); // I can't print this, but i can prove it works this way


    printf("\n");

// PCI testing
    printf("Discovered PCI devices:\n");
    pci_enumerate_bus_and_print();
    
    printf("\n");

// Network card driver testing

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

    printf("\n");

    //panic("Stop early");
    
// exit / fail test

    extern u64 timer_ticks;
    printf("timer_ticks completed = %i\n", timer_ticks);
    if (timer_ticks < 10) {
        printf("Theoretically this means we took 0.00%is to execute\n", timer_ticks);
    } else if (timer_ticks < 100) {
        printf("Theoretically this means we took 0.0%is to execute\n", timer_ticks);
    } else if (timer_ticks < 1000) {
        printf("Theoretically this means we took 0.%is to execute\n", timer_ticks);
    } else {
        printf("Theoretically this means we took a long time to execute\n");
    }

    printf("\n");

// prove memcpy works
#if 0
   static usize from_array[1000] = {
        [50] = 0x1234,
        [500] = 0x12345,
        [999] = 0x12346,
    };
    static usize to_array[1000] = {0};
    memcpy(&to_array, &from_array, sizeof(usize) * 1000);
    assert(to_array[50] == 0x1234, "memcpy does not work");
    assert(to_array[500] == 0x12345, "memcpy does not work");
    assert(to_array[999] == 0x12346, "memcpy does not work");
    printf("to_array[999] = 0x%x", to_array[999]);
#endif

    // test page fault
    // volatile int *x = (int *)0x1000000;
    // *x = 1;
    
// test long and short printf
    printf("%hhx %hx %x %lx\n", (u8)0xFF, (u16)0xFFFF, (u32)0xFFFFFFFF, (u64)0xFFFFFFFFFFFFFFFF);

    printf("%lo\n", 0x1234567890);
    printf("%lb\n", 0x1234567890);

    Vector *v = new_vec(int);
    int i = 10;
    for (int i=0; i<1000; i++) {
        vec_push(v, &i);
        vec_push(v, &i);
        vec_push(v, &i);
    }
    print_vector(v);

// test processes
    proc_create(test_kernel_thread);
    proc_create(count_to_100);
    proc_top();

    while (timer_ticks < 500) {
        // Give time for the threads to do their thing
    }

    // test assert
    assert(timer_ticks == 100, "Test assert #%i", 1);

    panic("kernel_main tried to return!\n");
}

