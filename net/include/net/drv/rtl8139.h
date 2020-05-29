
#pragma once
#ifndef NG_DRV_RTL8139_H
#define NG_DRV_RTL8139_H

#include <basic.h>
#include <ng/pci.h>
#include <ng/cpu.h>
#include <net/core.h>
#include <net/if.h>

struct rtl8139_device {
        pci_address_t pci_addr;
        size_t rx_buffer_ix;
        uint8_t *rx_buffer;
        uint16_t io_base;
        int irq;
        int tx_slot;
};

struct net_device *net_rtl8139_create(pci_address_t);
ng_result net_rtl8139_send_packet(struct net_device *net, struct pkb *pk);
ng_result net_rtl8139_interrupt_handler(interrupt_frame *r);

extern struct net_drv_impl rtl8139_drv;

#endif // NG_DRV_RTL8139_H

