
#pragma once
#ifndef NIGHTINGALE_NET_IF_H
#define NIGHTINGALE_NET_IF_H

#include <basic.h>
#include <ng/net/ether.h>
#include <ng/drv/rtl8139.h>

enum if_type {
        IF_RTL8139,
};

/*
 * Basically a tagged union of the interface structs
 * each should contain enough information to be able to
 * run all the functions here
 */

struct net_if {
        int type;
        int id;

        struct mac_addr mac_addr;

        union {
                struct rtl8139_if rtl8139;
        };

        void (*send_packet)(struct net_if *nic, void *buf, size_t len);
};

/*
extern int net_top_id;

struct net_if *interfaces;
*/

static inline void send_packet(struct net_if *nic, void *data, size_t len) {
        nic->send_packet(nic, data, len);
}

#endif
