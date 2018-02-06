
#include <string.h>

#include <basic.h>
#include <multiboot2.h>

#include "debug.h"
#include "panic.h"
#include "terminal.h"
#include "print.h"
#include "cpu/pic.h"
#include "cpu/pit.h"
#include "cpu/portio.h"
#include "allocator.h"
#include "paging.h"
#include "pci.h"

#ifdef SINGLE_COMPILATION_UNIT
#include "pci.c"
#include "cpu/interrupt.c"
#include "cpu/pic.c"
#include "cpu/pit.c"
#include "cpu/uart.c"
#include "allocator.c"
#include "paging.c"
#include "print.c"
// #include "term_serial.c"
#include "term_vga.c"
#endif

void kernel_main(u32 mb_magic, usize mb_info) {

// initialization
    vga_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    vga_clear();
    uart_init(COM1);
    printf("Terminal Initialized\n");
    printf("UART Initialized\n");

    remap_pic();
    for (int i=0; i<16; i++) {
        if (i == 2) continue; // inter-PIC interrupt
        mask_irq(i); // This clearly does not work!
    }
    printf("PIC remapped\n");

    setup_interval_timer(1000);
    printf("Interval Timer Initialized\n");

    uart_enable_interrupt(COM1);
    printf("Serial Interrupts Initialized\n");

    enable_irqs();
    printf("IRQs Enabled\n");

    //heap_init();

// Multiboot
    printf("Multiboot magic: 0x%x\n", mb_magic);
    printf("Multiboot info*: %p\n", mb_info);

    assert(mb_magic == MULTIBOOT2_BOOTLOADER_MAGIC,
           "Hair on fire, bootloader must be multiboot2");

    multiboot_tag *tag;

    usize size = *(u32 *)mb_info;
    printf("Multiboot announced size %i\n\n", size);
    printf("Which makes the end %p\n", size + mb_info);
    usize first_free_page = (size + mb_info + 0xfff) & ~0xfff;
    usize last_free_page = 0;
    printf("first_free_page = %p\n", first_free_page);

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
                // HACK to fid the real memory
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

    //panic("exit early so i can read it properly");

// allocation 

    printf("Setup physical allocator: %p -> %p\n", first_free_page, last_free_page);
    phy_allocator_init(first_free_page, last_free_page);
    printf("Allocate page test: %p\n", phy_allocate_page());

    /*
    u8 *alloc_test0 = malloc(16);

    printf("\nalloc_test0 = %x\n", alloc_test0);

    for (i32 i=0; i<16; i++) {
        alloc_test0[i] = i;
    }

    // debug_print_mem(16, alloc_test0-4);
    // debug_dump(alloc_test0);

    free(alloc_test0);
    */

// page resolution and mapping
    usize resolved1 = page_resolve_vtop(0x101888);
    printf("resolved vma:%p to pma:%p\n", 0x101888, resolved1);

    usize *test_page_allocator = (usize *)0x123456789000;
    page_map_vtop((usize)test_page_allocator, phy_allocate_page());
    *test_page_allocator = 100;
    printf("*%p = %i\n", test_page_allocator, *test_page_allocator);

    printf("\n");

    // debug_dump(pointer_to_be);'

// u128 test
    printf("\n\n");
    u128 x128_test = 0;
    x128_test -= 1;
    // debug_dump(&x); // i can't print this, but i can prove it works this way

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

// PCI testing
    printf("\n");
    printf("Discovered PCI devices:\n");

    pci_enumerate_bus_and_print();
    // should go for some sort of depth-first approach perhaps, check for a bus adapter
    
    printf("\n\n");

// Network card driver testing

    u32 p = pci_find_device_by_id(0x8086, 0x100e);
    printf("Network card ID = ");
    pci_print_addr(p);

    printf("\n\n");

    //panic("Stop early");

// exit / fail test

    extern i64 timer_ticks;
    printf("\ntimer_ticks completed = %i\n", timer_ticks);
    if (timer_ticks > 9 && timer_ticks < 99) {
        printf("Theoretically this means we took 0.0%is to execute\n", timer_ticks);
    }

    /* // syscall
    asm volatile ("mov $1, %%rax" ::: "rax");
    asm volatile ("int $0x80");
    */

    /* // page fault
    volatile int *x = (int *)0x1000000;
    *x = 1;
    // yes I can
    */

    assert(false, "Test assert #%i", 1);
    

    // while (true);
    panic("kernel_main tried to return!\n");
}

