
#include <ng/basic.h>
#include <ng/debug.h>
#include <ng/malloc.h>
#include <ng/panic.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <ng/print.h>
#include <ng/string.h>
#include <ng/vmm.h>
#include <arch/memmap.h>
#include <arch/x86/cpu.h>
#include <arch/x86/pic.h>
#include <drv/rtl8139.h>
#include <stddef.h>
#include <stdint.h>

// struct mac_addr my_mac;
// uint32_t pci_addr;
// uint16_t iobase;
uint8_t *rx_buffer;

void rtl8139_irq_handler(interrupt_frame *r);

int net_top_id = 0;

struct rtl8139_if *the_interface;

struct rtl8139_if *init_rtl8139(uint32_t pci_addr) {
        struct rtl8139_if *rtl = malloc(sizeof(struct rtl8139_if));
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
        pic_irq_unmask(irq);

        // Pull MAC address
        // struct mac_addr my_mac = {0};
        // for (int off=0; off<6; off++) {
        //     uint8_t c = inb(iobase + off);
        //     my_mac.data[off] = c;
        // }
        // printf("rtl8139: my mac is (can't print MACs)");
        // print_mac_addr(my_mac);
        printf("\n");

        // Start the NIC
        //
        // TODO: a lot of these magic numbers could be #defines
        //
        outb(iobase + 0x52, 0);    // power on
        outb(iobase + 0x37, 0x10); // reset
        while (inb(iobase + 0x37) & 0x10) {
        } // await reset
        printf("rtl8139: card reset\n");

        rx_buffer = vmm_reserve(1 * 1024*1024);
        printf("rtl8139: rx_buffer = %#lx\n", rx_buffer);
        rtl->rx_buffer = (uintptr_t)rx_buffer;
        uintptr_t phy_buf = pmm_allocate_contiguous(16);
        vmm_map_range((uintptr_t)rx_buffer, phy_buf, 16 * 0x1000,
                      PAGE_PRESENT | PAGE_WRITEABLE);
        outd(iobase + 0x30, phy_buf);
        outw(iobase + 0x3c, 0x0005); // configure interrupts txok and rxok

        outd(iobase + 0x40, 0x600); // send larger DMA bursts
        outd(iobase + 0x44, 0x68f); // accept all packets + unlimited DMA

        outb(iobase + 0x37, 0x0c); // enable rx and tx

        rtl->tx_slot = 1;

        // TODO: have a network-generic handler that looks for
        // network cards on that irq that are ready for reading data
        extern void (*irq_handlers[16])(interrupt_frame *);
        irq_handlers[irq] = rtl8139_irq_handler;
        printf("rtl8139: handler = %#lx\n", rtl8139_irq_handler);
        printf("rtl8139: handler = %#lx\n", &rtl8139_irq_handler);

        return rtl;
}

void rtl8139_send_packet(struct rtl8139_if *rtl, void *data, size_t len) {
        if (len > 1500) { // ETH_MTU
                panic("Tried to send overside packet on rtl8139\n");
        }

        struct eth_hdr *eth = data;

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
        while (inb(rtl->io_base + ctrl_reg_off) & 0x100) {
        }
        // await send confirmation
        while (inb(rtl->io_base + ctrl_reg_off) & 0x400) {
        }

        // slots are 1, 2, 3, 4 - MUST be used in sequence
        rtl->tx_slot %= 4;
        rtl->tx_slot += 1;
}

void rtl8139_irq_handler(interrupt_frame *r) {
        static int rx_ix = 0;
        struct rtl8139_if *rtl = the_interface;

        uint16_t int_flag = inw(rtl->io_base + 0x3e);
        if (!int_flag) {
                // nothing to process, just EOI
                goto eoi;
        }
        if (!(int_flag & 0x0001)) {
                // no read to process, just ack
                goto ack_irq;
        }

        while (!(inb(rtl->io_base + 0x37) & 0x01)) {

                int flags = *(uint16_t *)&rx_buffer[rx_ix];
                int length = *(uint16_t *)&rx_buffer[rx_ix + 2];

                if (!(flags & 1)) {
                        // bad packet indicated
                        // maybe this is a bad thing
                        // guess we'll find out when it happens
                        // this is a good candidate for having low-pri debug
                        // prints
                        printf("bad packet indicated by rtl8139\n");
                } else {
                        // Handle inbound packet
                        // struct queue_object* qo = malloc(
                        //         sizeof(struct queue_object) +
                        //         sizeof(struct pkt_desc) +
                        //         length - 8);
                        // struct pkt_desc* pkt_desc = (struct
                        // pkt_desc*)&qo->data; pkt_desc->iface = 0, // iface;
                        // pkt_desc->len = length - 8;
                        // memcpy(&pkt_desc->data, rx_buffer + rx_ix + 4, length
                        // - 8); queue_enqueue(&incoming_packets, qo);
                }

                rx_ix += round_up(length + 4, 4);
                rx_ix %= 8192;

                outw(rtl->io_base + 0x38, rx_ix - 0x10);
        }

ack_irq:
        outw(rtl->io_base + 0x3e, int_flag); // acks irq
eoi:
        pic_send_eoi(r->interrupt_number - 32);
}
