
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
#include "network.h"

// struct mac_addr my_mac;
// uint32_t pci_addr;
// uint16_t iobase;
uint8_t *rx_buffer;

struct net_if* nic_by_irq[16] = {0};

void rtl8139_irq_handler(interrupt_frame *r);

// TODO: move this:
#if X86_64
# define NET_BUFFER 0xffffffff84000000
#elif I686
# define NET_BUFFER 0x84000000
#endif

int net_top_id = 0;

struct net_if *init_rtl8139(uint32_t pci_addr) {
    struct net_if *intf = malloc(sizeof(struct net_if));
    struct rtl8139_if *rtl = &intf->rtl8139;
    rtl->pci_addr = pci_addr;

    // Enable bus mastering:
    uint32_t cmd = pci_config_read(pci_addr + 0x04);
    pci_config_write(pci_addr + 0x04, cmd | 0x04);

    // Get IO space:
    uint32_t base = pci_config_read(pci_addr + 0x10);
    uint16_t iobase = base & ~1;
    rtl->io_base = iobase;

    // Enable IRQ
    uint32_t irq = pci_config_read(pci_addr + 0x3C);
    irq &= 0xFF;
    printf("rtl8139: using irq %i\n", irq);
    rtl->irq = irq;
    if (nic_by_irq[irq]) {
        panic("a NIC already exists using irq %i. There can't be two.", irq);
    }
    nic_by_irq[irq] = intf;
    pic_irq_unmask(irq);

    // Pull MAC address
    struct mac_addr my_mac = {0};
    for (int off=0; off<6; off++) {
        uint8_t c = inb(iobase + off);
        my_mac.data[off] = c;
    }
    intf->mac_addr = my_mac;
    printf("rtl8139: my mac is ");
    print_mac_addr(my_mac);
    printf("\n");

    // Start the NIC
    //
    // TODO: a lot of these magic numbers could be #defines
    //
    outb(iobase + 0x52, 0);              // power on
    outb(iobase + 0x37, 0x10);           // reset
    while (inb(iobase + 0x37) & 0x10) {} // await reset
    printf("rtl8139: card reset\n");

    rx_buffer = (void*)NET_BUFFER; // HACK TODO: virtual space allocator
    printf("rtl8139: rx_buffer = %#lx\n", rx_buffer);
    rtl->rx_buffer = (uintptr_t)rx_buffer;
    uintptr_t phy_buf = pmm_allocate_contiguous(16);
    vmm_map_range((uintptr_t)rx_buffer,
            phy_buf, 16 * 0x1000, PAGE_PRESENT | PAGE_WRITEABLE);
    outd(iobase + 0x30, phy_buf);
    outw(iobase + 0x3c, 0x0005);        // configure interrupts txok and rxok
    
    outd(iobase + 0x40, 0x600);         // send larger DMA bursts
    outd(iobase + 0x44, 0x68f);         // accept all packets + unlimited DMA

    outb(iobase + 0x37, 0x0c);          // enable rx and tx

    rtl->tx_slot = 1;

    // TODO: have a network-generic handler that looks for
    // network cards on that irq that are ready for reading data
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

    if (len > ETH_MTU) {
        panic("Tried to send overside packet on rtl8139\n");
    }

    struct rtl8139_if *rtl = &intf->rtl8139;

    struct eth_hdr *eth = data;
    eth->src_mac = intf->mac_addr;

    uintptr_t phy_data = vmm_virt_to_phy((uintptr_t)data);
    if (phy_data > 0xFFFFFFFF)
        panic("rtl8139 can't send packets from above physical 4G\n");

    uint16_t tx_addr_off = 0x20 + (rtl->tx_slot - 1) * 4;
    uint16_t ctrl_reg_off = 0x10 + (rtl->tx_slot - 1) * 4;

    outd(rtl->io_base + tx_addr_off, phy_data);
    outd(rtl->io_base + ctrl_reg_off, len);

    // TODO: could let this happen async and just make sure the descriptor
    // is done when we loop back around to it.

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
    struct net_if* iface = nic_by_irq[r->interrupt_number - 32];

    uint16_t int_flag = inw(iface->rtl8139.io_base + 0x3e);
    if (!int_flag) {
        // nothing to process, just EOI
        goto eoi;
    }
    if (!(int_flag & 0x0001)) {
        // no read to process, just ack
        goto ack_irq;
    }

    while (! (inb(iface->rtl8139.io_base + 0x37) & 0x01)) {

        int flags = *(uint16_t *)&rx_buffer[rx_ix];
        int length = *(uint16_t *)&rx_buffer[rx_ix + 2];

        if (!(flags & 1)) {
            // bad packet indicated
            // maybe this is a bad thing
            // guess we'll find out when it happens
            // this is a good candidate for having low-pri debug prints
            printf("bad packet indicated by rtl8139\n");
            goto ack_irq;
        }

        // TODO
        // don't do this work in the irq handler, queue packets for something
        // else (kernel thread?) to do the dispatch to later
        dispatch_packet(rx_buffer + rx_ix + 4, length - 8, iface);

        rx_ix += round_up(length + 4, 4);
        rx_ix %= 8192;

        outw(iface->rtl8139.io_base + 0x38, rx_ix - 0x10);
    }

ack_irq:
    outw(iface->rtl8139.io_base + 0x3e, int_flag); // acks irq
eoi:
    pic_send_eoi(r->interrupt_number - 32);
}

