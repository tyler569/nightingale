
#include <basic.h>
#include <debug.h>
#include <stddef.h>
#include <stdint.h>
#include <panic.h>
#include <pci.h>
#include <print.h>
#include <arch/x86/cpu.h>
#include <arch/x86/pic.h>
#include <pmm.h>
#include <vmm.h>
#include <malloc.h>
#include "net_if.h"
#include "ether.h"
#include "rtl8139.h"

struct mac_addr my_mac;
uint32_t pci_addr;
uint16_t iobase;
uint8_t *rx_buffer;

void rtl8139_irq_handler(interrupt_frame *r);

struct net_if *init_rtl8139(uint32_t pci_addr) {

    // if network card is nope, then panic
    
    struct net_if *intf = malloc(sizeof(struct net_if));
    struct rtl8139_if *rtl = &intf->rtl8139;
    rtl->pci_addr = pci_addr;


    // Enable bus mastering:
    uint32_t cmd = pci_config_read(pci_addr + 0x04);
    pci_config_write(pci_addr + 0x04, cmd | 0x04);


    // Get IO space:
    uint32_t base = pci_config_read(pci_addr + 0x10);
    iobase = base & ~1;
    rtl->io_base = iobase;


    // Enable IRQ
    uint32_t irq = pci_config_read(pci_addr + 0x3C);
    irq &= 0xFF;
    printf("rtl8139: using irq %i\n", irq);
    rtl->irq = irq;
    pic_irq_unmask(irq);


    // Pull MAC address
    for (int off=0; off<6; off++) {
        uint8_t c = inb(iobase + off);
        my_mac.data[off] = c;
    }
    intf->mac_addr = my_mac;
    printf("rtl8139: my mac is ");
    print_mac_addr(my_mac);
    printf("\n");


    // Start the NIC
    outb(iobase + 0x52, 0);             // power on
    outb(iobase + 0x37, 0x10);          // reset
    while (inb(iobase + 0x37) & 0x10) {} // await reset
    printf("rtl8139: card reset\n");


    rx_buffer = malloc(8192 + 16 + 1500);
    printf("rtl8139: rx_buffer = %#lx\n", rx_buffer);
    rtl->rx_buffer = (uintptr_t)rx_buffer;
    outd(iobase + 0x30, vmm_virt_to_phy((uintptr_t)rx_buffer));
    outw(iobase + 0x3c, 0x0005); // configure interrupts txok and rxok
    
    outd(iobase + 0x40, 0x600); // send larger DMA bursts
    outd(iobase + 0x44, 0x68f); // accept all packets + unlimited DMA

    outb(iobase + 0x37, 0x0c); // enable rx and tx

    rtl->tx_slot = 1;

    //
    // TODO: have a network-generic handler that looks for
    // network cards on that irq that are ready for reading data
    //
    extern void (*irq_handlers[16])(interrupt_frame *);
    irq_handlers[irq] = rtl8139_irq_handler;
    printf("rtl8139: handler = %#lx\n", rtl8139_irq_handler);
    printf("rtl8139: handler = %#lx\n", &rtl8139_irq_handler);

    intf->id = net_top_id++;
    intf->type = IF_RTL8139;
    intf->send_packet = rtl8139_send_packet;

    return intf;
}

void rtl8139_send_packet(struct net_if *intf, void *data, size_t len) {

    if (len > ETH_MTU)
        panic("Tried to send overside packet on rtl8139\n");

    struct rtl8139_if *rtl = &intf->rtl8139;

    struct eth_hdr *eth = data;
    eth->src_mac = intf->mac_addr;

    uintptr_t phy_data = vmm_virt_to_phy((uintptr_t)data);
    if (phy_data > 0xFFFFFFFF)
        panic("rtl8139 can't send packets from above physical 4G\n");

    uint16_t tx_addr_off = 0x20 + (rtl->tx_slot - 1) * 4;
    uint16_t ctrl_reg_off = 0x10 + (rtl->tx_slot - 1) * 4;

    printf("sending packet at vma:%#lx, pma:%#lx, len:%i\n", data, phy_data, len);
    outd(rtl->io_base + tx_addr_off, phy_data);
    outd(rtl->io_base + ctrl_reg_off, len);

    // await device taking packet
    while (inb(rtl->io_base + ctrl_reg_off) & 0x100) {}
    // await send confirmation
    while (inb(rtl->io_base + ctrl_reg_off) & 0x400) {}


    // slots are 1, 2, 3, 4 - MUST be used in sequence
    rtl->tx_slot %= 4;
    rtl->tx_slot += 1;
}

void rtl8139_irq_handler(interrupt_frame *r) {
    static int rx_ix = 0;

    uint16_t int_flag = inw(iobase + 0x3e);
    if (!int_flag) {
        // nothing to process, just EOI
        goto eoi;
    }
    if (!int_flag & 0x0001) {
        // no read to process, just ack
        goto ack_irq;
    }

    //int rx_ix = inw(iobase + 0x34);
    
    while (! (inb(iobase + 0x37) & 0x01)) {

        printf("rtl8139: received a packet at rx_buffer:%#lx\n", rx_ix);
        dump_mem(rx_buffer, rx_ix + 500);

        // debug_dump(rx_buffer + rx_ix);

        int flags = *(uint16_t *)&rx_buffer[rx_ix];
        int length = *(uint16_t *)&rx_buffer[rx_ix + 2];

        printf("  flags: %#x, length: %i\n", flags, length);

        if (!flags & 1) {
            printf("Packet descriptor does not indicate it was good... ignoring\n");
            panic();
        }

        printf("\n");

        rx_ix += length + 4;
        rx_ix += 3;
        rx_ix &= ~3; // round up to multiple of 4
        rx_ix %= 8192;

        outw(iobase + 0x38, rx_ix - 0x10);
    }

ack_irq:
    outw(iobase + 0x3e, int_flag); // acks irq
eoi:
    pic_send_eoi(r->interrupt_number - 32);
}

