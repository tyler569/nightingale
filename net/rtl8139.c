#include <basic.h>
#include <net/core.h>
#include <net/if.h>
#include <net/drv/rtl8139.h>
#include <ng/debug.h>
#include <assert.h>
#include <stdio.h>

typedef int ng_result;

struct net_drv_impl rtl8139_drv = {
        .send = net_rtl8139_send_packet,
};

struct net_device *net_rtl8139_create(pci_address_t addr) {
        UNREACHABLE();
}

ng_result net_rtl8139_send_packet(struct net_device *dev, struct pkb *pk) {
        UNREACHABLE();
}

ng_result net_rtl8139_interrupt_handler(interrupt_frame *r) {
        UNREACHABLE();
}

