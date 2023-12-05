#pragma once
#ifndef NG_RTL8139_H
#define NG_RTL8139_H

#include <ng/drv/pci_device.h>
#include <stddef.h>

class rtl8139 : public pci_device {
    // int io_base;
    // int tx_slot;
    // rx ring info

public:
    explicit rtl8139(pci_address pci_address);
    ~rtl8139() = default;
    rtl8139(const rtl8139 &) = delete;
    rtl8139(rtl8139 &&) = delete;
    rtl8139 &operator=(const rtl8139 &) = delete;
    rtl8139 &operator=(rtl8139 &&) = delete;

    void init();
    void send_packet(const void *data, size_t len);
    void recv_packet(void *data, size_t len);
};

#endif // NG_RTL8139_H
