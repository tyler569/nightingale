#pragma once

#include "nic.h"
#include "pci.h"
#include <stddef.h>

class nic_rtl8139 : public nic {
public:
    static constexpr uint16_t vendor_id = 0x10ec;
    static constexpr uint16_t device_id = 0x8139;

    explicit nic_rtl8139(pci_address addr);
    ~nic_rtl8139() override = default;
    nic_rtl8139(const nic_rtl8139 &) = delete;
    nic_rtl8139(nic_rtl8139 &&) = delete;
    nic_rtl8139 &operator=(const nic_rtl8139 &) = delete;
    nic_rtl8139 &operator=(nic_rtl8139 &&) = delete;

    int interrupt_number();
    void send_packet(const void *data, size_t len) override;
    ssize_t recv_packet(void *data, size_t len) override;
    void interrupt_handler();

private:
    pci_address m_pci_address;
    uint64_t m_mmio_base {};
    uint16_t m_io_base {};
    uint8_t m_mac_address[6] {};
    int m_tx_slot { 0 };

    void *m_rx_buffer {};
    phys_addr_t m_rx_buffer_phys {};
    size_t m_rx_index { 0 };

    void debug_print();

    void reset();
    void set_rx_buffer();
    void setup_txrx();
    void enable_interrupts(int intr);
    void disable_interrupts(int intr);
    void enable_txrx();
    uint32_t tx_status();
    void tx_write(uint32_t data, size_t len);
    uint16_t ack_interrupts();
    bool rx_empty();

    uint16_t *read_packet_header()
    {
        return (uint16_t *)((uint8_t *)m_rx_buffer + m_rx_index);
    }

    static constexpr size_t rx_buffer_pages = 17;
    static constexpr size_t rx_buffer_size = rx_buffer_pages * 4096;

    static constexpr uint8_t reg_id = 0x00;
    static constexpr uint8_t reg_tx_status_0 = 0x10;
    static constexpr uint8_t reg_tx_addr_0 = 0x20;
    static constexpr uint8_t reg_rx_buffer = 0x30;
    static constexpr uint8_t reg_command = 0x37;
    static constexpr uint8_t reg_capr = 0x38;
    static constexpr uint8_t reg_intr_mask = 0x3c;
    static constexpr uint8_t reg_intr_status = 0x3e;
    static constexpr uint8_t reg_tx_config = 0x40;
    static constexpr uint8_t reg_rx_config = 0x44;
    static constexpr uint8_t reg_config_0 = 0x51;
    static constexpr uint8_t reg_config_1 = 0x52;

    static constexpr uint8_t cmd_rx_buf_empty = 0x01;
    static constexpr uint8_t cmd_tx_enable = 0x04;
    static constexpr uint8_t cmd_rx_enable = 0x08;
    static constexpr uint8_t cmd_reset = 0x10;

    static constexpr uint32_t tx_config_dma_all = 0x700;
    static constexpr uint32_t tx_config_crc = 0x1000;

    static constexpr uint32_t rx_config_accept_all = 0x000f;
    static constexpr uint32_t rx_config_wrap = 0x0080;
    static constexpr uint32_t rx_config_dma_all = 0x0700;
    static constexpr uint32_t rx_config_16k = 0x0800;

    static constexpr uint16_t intr_rx_ok = 0x01;
    static constexpr uint16_t intr_rx_err = 0x02;
    static constexpr uint16_t intr_tx_ok = 0x04;
    static constexpr uint16_t intr_tx_err = 0x08;

    static constexpr uint32_t tx_own = 0x100;
    static constexpr uint32_t tx_ok = 0x400;
};
