
#pragma once
#ifndef NG_DRV_RTL8139_H
#define NG_DRV_RTL8139_H

#include <basic.h>
#include <ng/pci.h>
#include <stddef.h>
#include <stdint.h>
#include <ng/net/ether.h>
#include <ng/net/net_if.h>

struct rtl8139_if {
        struct mac_addr mac_addr;
        uintptr_t rx_buffer;
        size_t rx_buffer_ix;
        uint32_t pci_addr;
        uint16_t io_base;
        int irq;
        int tx_slot;
};

void rtl8139_init(uint32_t pci_addr);

void rtl8139_send_packet(struct net_if *rtl, void *data, size_t len);

#endif // NG_DRV_RTL8139_H

