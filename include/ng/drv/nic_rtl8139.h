#pragma once

#include <ng/drv/pci.h>
#include <stddef.h>

class nic_rtl8139 {
    pci_address m_pci_address;
    uint64_t m_mmio_base;

public:
    static constexpr uint16_t vendor_id = 0x10EC;
    static constexpr uint16_t device_id = 0x8139;

    explicit nic_rtl8139(pci_address pci_address);
    ~nic_rtl8139() = default;
    nic_rtl8139(const nic_rtl8139 &) = delete;
    nic_rtl8139(nic_rtl8139 &&) = delete;
    nic_rtl8139 &operator=(const nic_rtl8139 &) = delete;
    nic_rtl8139 &operator=(nic_rtl8139 &&) = delete;

    void init();
    void send_packet(const void *data, size_t len);
    void recv_packet(void *data, size_t len);
};
