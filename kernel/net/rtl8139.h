
#pragma once
#ifndef NIGHTINGALE_NET_RTL8139_H
#define NIGHTINGALE_NET_RTL8139_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include "ether.h"

struct rtl8139_if {
    struct mac_addr mac_addr;
    uintptr_t rx_buffer;
    size_t rx_buffer_ix;
    uint32_t pci_id;
    uint16_t io_base;
    int tx_slot;
};

struct rtl8139_if *init_rtl8139(void);

void send_packet(struct rtl8139_if *intf, void *data, size_t len);

#endif

