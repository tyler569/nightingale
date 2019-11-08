
#pragma once
#ifndef NG_NET_NET_IF_H
#define NG_NET_NET_IF_H

#include <basic.h>
#include <ng/net/ether.h>
#include <ng/net/ip.h>
#include <ng/drv/rtl8139.h>
#include <ng/net/loopback.h>

enum if_type {
        IF_RTL8139,
        LOOPBACK,
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
        uint32_t ip_addr;
        uint8_t ip6_addr[16];

        union {
                struct rtl8139_if rtl8139;
                struct loopback loopback;
        };

        void (*send_packet)(struct net_if *nic, void *buf, size_t len);
};

/*
extern int net_top_id;

struct net_if *interfaces;
*/

#endif // NG_NET_NET_IF_H

