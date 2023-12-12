#include "assert.h"
#include <ng/drv/nic_rtl8139.h>
#include <ng/pmm.h>
#include <ng/vmm.h>
#include <stdio.h>

nic_rtl8139::nic_rtl8139(pci_address pci_address)
    : m_pci_address(pci_address)
{
    printf("nic_rtl8139: constructor\n");
    uint32_t bar0 = pci_read32(m_pci_address, PCI_BAR0);
    uint32_t bar1 = pci_read32(m_pci_address, PCI_BAR1);
    if ((bar0 & 1) == 1) {
        m_io_base = bar0 & ~0xf;
    } else {
        printf("nic_rtl8139: bar0 is not an io port\n");
    }
    if ((bar1 & 1) == 0) {
        m_mmio_base = (bar1 & ~0xf) | HW_MAP_BASE;
    } else {
        printf("nic_rtl8139: bar1 is not an mmio address\n");
    }

    for (int i = 0; i < 6; ++i) {
        m_mac_address[i] = mmio_read8(m_mmio_base, i);
    }

    pci_enable_bus_mastering(m_pci_address);
    reset();
    set_rx_buffer();
    enable_interrupts(intr_rx_ok | intr_tx_ok);
    enable_txrx();
    debug_print();
}

void nic_rtl8139::debug_print()
{
    printf("nic_rtl8139: io_base: %#x\n", m_io_base);
    printf("nic_rtl8139: mmio_base: %#lx\n", m_mmio_base);

    printf("nic_rtl8139: mac address: %02x:%02x:%02x:%02x:%02x:%02x\n",
        m_mac_address[0], m_mac_address[1], m_mac_address[2], m_mac_address[3],
        m_mac_address[4], m_mac_address[5]);
}

void nic_rtl8139::reset()
{
    mmio_write8(m_mmio_base, reg_config_1, 0x0);
    mmio_write8(m_mmio_base, reg_command, cmd_reset);
    while ((mmio_read8(m_mmio_base, reg_command) & cmd_reset) != 0) {
        __asm__ volatile("pause");
    }
}

void nic_rtl8139::set_rx_buffer()
{
    if (m_rx_buffer_phys == 0 && m_rx_buffer == nullptr) {
        m_rx_buffer = vmm_reserve(rx_buffer_size);
        m_rx_buffer_phys = pm_alloc_contiguous(rx_buffer_pages);
        vmm_map_range((uintptr_t)m_rx_buffer, m_rx_buffer_phys, rx_buffer_size,
            PAGE_PRESENT | PAGE_WRITEABLE);
    }

    mmio_write32(m_mmio_base, reg_rx_buffer, m_rx_buffer_phys);
}

void nic_rtl8139::enable_interrupts(int intr)
{
    auto interrupts = mmio_read16(m_mmio_base, reg_intr_mask);
    mmio_write16(m_mmio_base, reg_intr_mask, interrupts | intr);
}

void nic_rtl8139::disable_interrupts(int intr)
{
    auto interrupts = mmio_read16(m_mmio_base, reg_intr_mask);
    mmio_write16(m_mmio_base, reg_intr_mask, interrupts & ~intr);
}

void nic_rtl8139::enable_txrx()
{
    mmio_write8(m_mmio_base, reg_command, cmd_rx_enable | cmd_tx_enable);
}

uint32_t nic_rtl8139::tx_status()
{
    return mmio_read32(m_mmio_base, reg_tx_status_0 + m_tx_slot * 4);
}

void nic_rtl8139::tx_write(uint32_t data, size_t len)
{
    mmio_write32(m_mmio_base, reg_tx_addr_0 + m_tx_slot * 4, data);
    mmio_write32(m_mmio_base, reg_tx_status_0 + m_tx_slot * 4, len);
}

uint16_t nic_rtl8139::ack_interrupts()
{
    return mmio_read16(m_mmio_base, reg_intr_status);
}

bool nic_rtl8139::rx_empty()
{
    return (mmio_read8(m_mmio_base, reg_command) & cmd_rx_buf_empty) != 0;
}

int nic_rtl8139::interrupt_number()
{
    return pci_read8(m_pci_address, PCI_INTERRUPT_LINE);
}

void nic_rtl8139::send_packet(const void *data, size_t len)
{
    printf("nic_rtl8139: send_packet: len %zu\n", len);

    while ((tx_status() & tx_own) != 0) {
        // this descriptor is still busy. the nic will
        // set the own bit when it's done with it.
        __asm__ volatile("pause");
    }

    phys_addr_t data_phy = vmm_virt_to_phy((uintptr_t)data);
    tx_write(data_phy, len);

    m_tx_slot = (m_tx_slot + 1) % 4;
}

ssize_t nic_rtl8139::recv_packet(void *data, size_t len)
{
    uint16_t *header = read_packet_header();
    auto flags = header[0];
    auto length = header[1];

    printf("nic_rtl8139: recv_packet: flags %04x len %i\n", flags, length);

    ssize_t result = -1;
    if ((flags & 1) == 0) {
        printf("nic_rtl8139: bad packet\n");
    } else {
        size_t to_copy = MIN(len, length - 8);
        memcpy(data, (uint8_t *)m_rx_buffer + m_rx_index + 4, to_copy);
        result = static_cast<ssize_t>(to_copy);
    }

    m_rx_index += ROUND_UP(length + 4, 4);
    m_rx_index %= rx_buffer_size;
    mmio_write16(m_mmio_base, reg_capr, m_rx_index - 0x10);

    return result;
}

void print_packet(const void *data, size_t len)
{
    printf("nic_rtl8139: received packet of length %zu\n", len);
    for (int i = 0; i < len; ++i) {
        printf("%02x ", ((uint8_t *)data)[i]);
        if (i % 16 == 15 && i != len - 1) {
            printf("\n");
        }
    }
    printf("\n\n");
}

void nic_rtl8139::interrupt_handler()
{
    printf("nic_rtl8139: interrupt_handler\n");

    uint16_t int_flag = ack_interrupts();
    if (int_flag == 0) {
        // no interrupt to handle
        return;
    }

    if (!(int_flag & 1)) {
        // no packet to receive
        return;
    }

    while (!rx_empty()) {
        char buffer[2048];
        ssize_t len = recv_packet(buffer, sizeof(buffer));
        if (len > 0) {
            print_packet(buffer, len);
        } else {
            printf("nic_rtl8139: bad packet\n");
        }
    }
}