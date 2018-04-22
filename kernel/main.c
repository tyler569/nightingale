
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
#include <rand.h>
#include <mutex.h>
#include <arch/x86/vga.h>
#include <arch/x86/pic.h>
#include <arch/x86/pit.h>
// acpi testing
// #include <arch/x86/acpi.h>
// apic testing
// #include <arch/x86/apic.h>
#include <arch/x86/cpu.h>
// network testing
#include <net/rtl8139.h>
#include <net/ether.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/inet.h>
#include <elf.h>
#include <fs/vfs.h>

int net_top_id = 0; // TODO: put this somewhere sensible

void test_thread() {
    for (int j=0; j<10000; j++) {}
    
    int foo;
    printf("thread %i @ %#lx\n", current_kthread->id, &foo);

    exit_kthread();
}

void kernel_main(uint32_t mb_magic, uintptr_t mb_info) {

// initialization
    mb_info += 0xffffffff80000000; // virtual memory offset

    // UNMAP INITIAL LOW P4 ENTRY
    *vmm_get_p3_entry(0) = 0;
    *vmm_get_p4_entry(0) = 0;

    vga_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    vga_clear();
    uart_init(COM1);
    printf("terminal: initialized\n");
    printf("uart: initialized\n");

    rand_add_entropy(0xdeadbeef13378008);
    printf("rand: initialized 'random' generator");

    install_isrs();
    printf("idt: interrupts installed\n");

    pic_init(); // leaves everything masked
    pic_irq_unmask(0); // Allow timer though
    printf("pic: remapped and masked\n");

    int timer_interval = 100; // per second
    set_timer_periodic(timer_interval);
    printf("pit: running at %i/s\n", timer_interval);

    uart_enable_interrupt(COM1);
    pic_irq_unmask(4); // Allow serial interrupt
    printf("uart: listening for interrupts\n");
    pic_irq_unmask(1); // Allow keyboard interrupt
    printf("kbrd: listening for interrupts\n");

    enable_irqs();
    printf("cpu: allowing irqs\n");

    if (mb_magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        panic("Bootloader does not appear to be multiboot2.");
    mb_parse(mb_info);
    mb_mmap_print();

    size_t memory = mb_mmap_total_usable();
    printf("mmap: total usable memory: %lu (%luMB)\n",
            memory, memory / (1024 * 1024));

    size_t size = *(uint32_t *)mb_info;
    if (size + mb_info >= 0xffffffff801c0000)
        panic("Multiboot data structure overlaps hard-coded start of heap!");

    Elf64_Ehdr *program = (void *)mb_get_initfs();
    printf("mb: user init at %#lx\n", program);

    // pretty dirty thing - just saying "memory starts after multiboot"...
    // TODO: Cleanup
    uintptr_t first_free_page = ((uintptr_t)program + 0x10fff) & ~0xfff;
    first_free_page -= 0xffffffff80000000; // vm-phy offset

    printf("pmm: using %#lx as the first physical page\n", first_free_page);

    // So we have something working in the meantime
    pmm_allocator_init(first_free_page, 0x2000000); // TEMPTEMPTEMPTEMP

    init_vfs();
    printf("vfs: filesystem initiated\n");

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

#ifdef __TEST_MUTEX
    static kmutex test_mutex = KMUTEX_INIT;

    printf("try acquire test mutex: %i\n", try_acquire_mutex(&test_mutex));
    printf("try acquire test mutex: %i\n", try_acquire_mutex(&test_mutex));
    printf("release test mutex: %i\n", release_mutex(&test_mutex));

    printf("acquire test mutex: %i\n", await_mutex(&test_mutex));
    printf("try acquire test mutex: %i\n", try_acquire_mutex(&test_mutex));
    printf("release test mutex: %i\n", release_mutex(&test_mutex));

    printf("acquire test mutex: %i\n", await_mutex(&test_mutex));
#endif

    pci_enumerate_bus_and_print();

    printf("\n");
    printf("Project Nightingale\n");
    printf("\n");

    extern volatile uint64_t timer_ticks;
    printf("This took %i ticks so far\n", timer_ticks);

    create_kthread(thread_watchdog);

#if __TEST_THREADS
    create_kthread(test_thread);
    create_user_thread(test_user_thread);
    kthread_top();
#endif
    
#if 0
    uint32_t rtl_nic_addr = pci_find_device_by_id(0x10ec, 0x8139);
    if (rtl_nic_addr == ~0) {
        printf("no rtl8139 found, aborting network test\n");
    } else {

        struct net_if *nic = init_rtl8139(rtl_nic_addr);

        struct eth_hdr *arp_req = calloc(ETH_MTU, 1);
        size_t len = make_ip_arp_req(arp_req, nic->mac_addr, 0x0a00020f, 0x0a000202);

        len = (len > 64) ? len : 64;

        print_arp_pkt((void *)&arp_req->data);
        send_packet(nic, arp_req, len);
        //free(arp_req);
        
        struct mac_addr gw_mac = {{ 0x52, 0x55, 0x0a, 0x00, 0x02, 0x02 }};
        struct mac_addr zero_mac = {{ 0, 0, 0, 0, 0, 0 }};

#define PINGDL 64

        void *ping = calloc(ETH_MTU, 1);
        len = make_eth_hdr(ping, gw_mac, zero_mac, ETH_IP);
        struct eth_hdr *ping_frame = ping;
        //len += make_ip_hdr(ping + len, 0x4050, PROTO_ICMP, 0x0a000202);
        len += make_ip_hdr(ping + len, 0x4050, PROTO_ICMP, 0x0a000202);
        struct ip_hdr *ping_packet = (void *)&ping_frame->data;
        len += make_icmp_req(ping + len, 0xaa, 1);
        struct icmp_pkt *ping_msg = (void *)&ping_packet->data;
        len += PINGDL;
        ping_packet->total_len = htons(len - sizeof(struct eth_hdr));

        for (int i=0; i<3; i++) {
            memset(ping + len - PINGDL, 0x41 + i, PINGDL);

            ping_packet->hdr_checksum = 0;
            ping_msg->checksum = 0;
            place_ip_checksum(ping_packet);
            place_icmp_checksum(ping_msg, PINGDL);

            send_packet(nic, ping, len);
        }
    }
#endif

    printf("\n");

    printf("test random: %x\n", rand_get());


    // also acts as __strong_heap_protection litmus against net code above
    int *foo = malloc(0x80000);
    foo = malloc(0x80000);
    printf("test unbacked memory: malloc at %#lx ... ", foo);
    *foo = 10;
    if (*foo == 10) {
        printf("works!\n");
    } else {
        printf("didn't work *and* didn't fault? umm...\n");
    }

    load_elf(program);
    printf("\n\nStarting ring 3 thread:\n\n");

    create_user_thread(program->e_entry);

    // kthread_top();

    while (true) {
        asm volatile ("hlt");
    }

    printf("That took %i ticks\n", timer_ticks);

    // *(volatile int *)0x5123213213 = 100; // testing backtrace

    panic("kernel_main tried to return!");
}

