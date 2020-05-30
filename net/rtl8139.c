#include <basic.h>
#include <net/core.h>
#include <net/if.h>
#include <net/drv/rtl8139.h>
#include <ng/debug.h>
#include <ng/pmm.h>
#include <ng/vmm.h>
#include <ng/x86/pic.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef int ng_result;

struct net_drv_impl rtl8139_drv = {
        .send = net_rtl8139_send_packet,
};

static void enable_bus_mastering(pci_address_t addr) {
        uint32_t state = pci_config_read(addr + 4);
        pci_config_write(addr + 4, state | 0x04);
}

static uint16_t get_io_base(pci_address_t addr) {
        return pci_config_read(addr + 0x10) & ~1;
}

static int get_irq_line(pci_address_t addr) {
        return pci_config_read(addr + 0x3C) & 0xFF;
}

static struct mac_address get_mac_address(struct rtl8139_device *rtl) {
        struct mac_address mac = {0};
        for (int i=0; i<6; i++) {
                mac.data[i] = inb(rtl->io_base + i);
        }
        return mac;
}

static void rtl8139_init(struct rtl8139_device *rtl) {
        uint16_t iobase = rtl->io_base;
                outb(iobase + 0x52, 0);    // power on
        outb(iobase + 0x37, 0x10); // reset
        while (inb(iobase + 0x37) & 0x10) {
                // await reset
        }

        uint8_t *rx_buffer = vmm_reserve(16 * 4096);
        uintptr_t phy_buf = pmm_allocate_contiguous(16);
        vmm_map_range((uintptr_t)rx_buffer, phy_buf, 16 * 0x1000,
                      PAGE_PRESENT | PAGE_WRITEABLE);
        rtl->rx_buffer = rx_buffer;

        outd(iobase + 0x30, phy_buf);
        outw(iobase + 0x3c, 0x0005); // configure interrupts txok and rxok

        outd(iobase + 0x40, 0x600); // send larger DMA bursts
        outd(iobase + 0x44, 0x68f); // accept all packets + unlimited DMA

        outb(iobase + 0x37, 0x0c); // enable rx and tx
}

struct net_device *net_rtl8139_create(pci_address_t addr) {
        struct net_device *dev = zmalloc(sizeof(struct net_device));
        struct rtl8139_device *rtl = zmalloc(sizeof(struct rtl8139_device));
        dev->device_impl = rtl;
        dev->type = RTL8139;
        
        rtl->pci_addr = addr;
        enable_bus_mastering(addr);

        uint16_t iobase = get_io_base(addr);
        rtl->io_base = iobase;

        uint32_t irq = get_irq_line(addr);
        rtl->irq = irq;
        pic_irq_unmask(irq);

        struct mac_address mac = get_mac_address(rtl);
        dev->mac_address = mac;

        rtl->tx_slot = 1;

        rtl8139_init(rtl);
        return dev;
}

ng_result net_rtl8139_send_packet(struct net_device *dev, struct pkb *pk) {
        assert(dev->type == RTL8139);
        struct rtl8139_device *rtl = dev->device_impl;

        assert(pk->length > 0 && pk->length < ETH_MTU);
        virt_addr_t data = (virt_addr_t)pk->buffer;
        phys_addr_t phy_data = vmm_virt_to_phy(data);

        assert(phy_data < 0xFFFFFFFF); // 32 bit DMA woot

        uint16_t tx_addr_off = 0x20 + (rtl->tx_slot - 1) * 4;
        uint16_t ctrl_reg_off = 0x10 + (rtl->tx_slot - 1) * 4;

        outd(rtl->io_base + tx_addr_off, phy_data);
        outd(rtl->io_base + ctrl_reg_off, pk->length);

        // TODO: could let this happen async and just make sure the descriptor
        // is done when we loop back around to it.

        // await device taking packet
        while (inb(rtl->io_base + ctrl_reg_off) & 0x100);
        // await send confirmation
        while (inb(rtl->io_base + ctrl_reg_off) & 0x400);

        // slots are 1, 2, 3, 4 - MUST be used in sequence
        rtl->tx_slot %= 4;
        rtl->tx_slot += 1;
        return 0;
}

ng_result net_rtl8139_interrupt_handler(interrupt_frame *r) {
        UNREACHABLE();
}

