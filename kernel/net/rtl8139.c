
#include <basic.h>
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
#include "ether.h"
#include "rtl8139.h"

struct mac_addr my_mac;
uint32_t pci_addr;
uint16_t iobase;

// uint8_t rx_buffer[8192 + 16]; // TEMP: get this from pmm later
uint8_t *rx_buffer;

void rtl8139_irq_handler(interrupt_frame *r);

struct rtl8139_if *init_rtl8139() {
    uint32_t rtl = pci_find_device_by_id(0x10ec, 0x8139);
    pci_addr = rtl;
    printf("Network card ID = ");
    // if network card is nope, then panic
    
    struct rtl8139_if *intf = malloc(sizeof(struct rtl8139_if));
    intf->pci_id = rtl;

    pci_print_addr(rtl);
    printf("\n");


    // Enable bus mastering:
    uint32_t cmd = pci_config_read(rtl + 0x04);
    pci_config_write(rtl + 0x04, cmd | 0x04);


    // Get IO space:
    uint32_t base = pci_config_read(rtl + 0x10);
    iobase = base & ~1;
    intf->io_base = iobase;


    // Enable IRQ
    uint32_t irq = pci_config_read(rtl + 0x3C);
    irq &= 0xFF;
    printf("rtl8139: using irq %i\n", irq);
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


    rx_buffer = malloc(8192 + 16);
    printf("rtl8139: rx_buffer = %#lx\n", rx_buffer);
    intf->rx_buffer = (uintptr_t)rx_buffer;
    outd(iobase + 0x30, vmm_virt_to_phy((uintptr_t)rx_buffer));
    outw(iobase + 0x3c, 0x0005); // configure interrupts txok and rxok
    
    outd(iobase + 0x40, 0x700); // send larger DMA bursts
    outd(iobase + 0x44, 0x78f); // accept all packets + unlimited DMA

    outb(iobase + 0x37, 0x0c); // enable rx and tx

    intf->tx_slot = 1;

    extern void (*irq_handlers[16])(interrupt_frame *);
    irq_handlers[11] = rtl8139_irq_handler;

    return intf;
}

void send_packet(struct rtl8139_if *intf, void *data, size_t len) {

    if (len > ETH_MTU)
        panic("Tried to send overside packet on rtl8139\n");

    struct eth_hdr *eth = data;
    eth->src_mac = intf->mac_addr;

    uintptr_t phy_data = vmm_virt_to_phy((uintptr_t)data);
    if (phy_data > 0xFFFFFFFF)
        panic("rtl8139 can't send packets from above physical 4G\n");

    uint16_t tx_addr_off = 0x20 + (intf->tx_slot - 1) * 4;
    uint16_t ctrl_reg_off = 0x10 + (intf->tx_slot - 1) * 4;

    printf("sending packet at vma:%#lx, pma:%#lx, len:%i\n", data, phy_data, len);
    outd(intf->io_base + tx_addr_off, phy_data);
    outd(intf->io_base + ctrl_reg_off, len);

    // await device taking packet
    while (inb(intf->io_base + ctrl_reg_off) & 0x100) {}
    // await send confirmation
    while (inb(intf->io_base + ctrl_reg_off) & 0x400) {}

    intf->tx_slot %= 4;
    intf->tx_slot += 1;
}

void rtl8139_irq_handler(interrupt_frame *r) {
    uint16_t int_flag = inw(iobase + 0x3e);
    // check int flag of all nic on this irq line

    outw(iobase + 0x3e, int_flag); // acks irq
    printf("rtl8139: received a packet!\n");

    static int rx_ix = 0;
    printf("  flags: %i\n", *(uint16_t *)&rx_buffer[rx_ix]);
    printf("  length: %i\n", *(uint16_t *)&rx_buffer[rx_ix + 2]);
    rx_ix += *(uint16_t *)&rx_buffer[rx_ix + 2] + 4;
    rx_ix %= (8192 + 16);

    pic_send_eoi(r->interrupt_number - 32);
}

