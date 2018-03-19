
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <pci.h>
#include <print.h>
#include <arch/x86/cpu.h>
#include <arch/x86/pic.h>
#include <pmm.h>
#include <vmm.h>
#include <malloc.h>
#include "ether.h"
#include "rtl8139.h"

struct mac_addr my_mac;
uint32_t pci_addr;
uint16_t iobase;

// uint8_t rx_buffer[8192 + 16]; // TEMP: get this from pmm later
uint8_t rx_buffer;

int init_rtl8139() {
    uint32_t rtl = pci_find_device_by_id(0x10ec, 0x8139);
    pci_addr = rtl;
    printf("Network card ID = ");
    // if network card is nope, then panic

    pci_print_addr(rtl);
    printf("\n");
    printf("BAR0: %#010x\n", pci_config_read(rtl + 0x10));
    uint32_t base = pci_config_read(rtl + 0x10);
    uint32_t irq = pci_config_read(rtl + 0x3C);
    irq &= 0xFF;
    printf("rtl8139: using irq %i\n", irq);
    pic_irq_unmask(irq);
    /*
    printf("BAR1: %#010x\n", pci_config_read(network_card + 0x14));
    printf("BAR2: %#010x\n", pci_config_read(network_card + 0x18));
    printf("BAR3: %#010x\n", pci_config_read(network_card + 0x1c));
    printf("BAR4: %#010x\n", pci_config_read(network_card + 0x20));
    printf("BAR5: %#010x\n", pci_config_read(network_card + 0x24));
    */

    iobase = base & ~1;

    for (int off=0; off<6; off++) {
        uint8_t c = inb(iobase + off);
        my_mac.data[off] = c;
    }

    printf("rtl8139: my mac is ");
    print_mac_addr(my_mac);
    printf("\n");

    printf("rtl8139: card init\n");
    outb(iobase + 0x52, 0);     // power on
    outb(iobase + 0x37, 0x10);  // reset
    while (inb(iobase + 0x37) & 0x10) {} // await reset

    rx_buffer = malloc(8192 + 16);
    outd(iobase + 0x30, vmm_virt_to_phy((uintptr_t)rx_buffer));
    outw(iobase + 0x3c, 0x0005); // configure interrupts txok and rxok
    
    outd(iobase + 0x44, 0x4f); // accept all packets

    outb(iobase + 0x37, 0x0c); // enable rx and tx


    return 0;
}

void rtl8139_ack_irq() {
    outw(iobase + 0x3e, 1); // acks irq
}

