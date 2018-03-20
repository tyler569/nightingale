
#pragma once
#ifndef NIGHTINGALE_NET_RTL8139_H
#define NIGHTINGALE_NET_RTL8139_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <pci.h>
#include "ether.h"

struct rtl8139_if {
    struct mac_addr mac_addr;
    uintptr_t rx_buffer;
    size_t rx_buffer_ix;
    uint32_t pci_addr;
    uint16_t io_base;
    int irq;
    int tx_slot;
};

// previous has to be available for net_if.h
// is this a good solution? maybe? TODO: decide
#include "net_if.h"

struct net_if *init_rtl8139(uint32_t pci_addr);

void rtl8139_send_packet(struct net_if *intf, void *data, size_t len);

#endif

