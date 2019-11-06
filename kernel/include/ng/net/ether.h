
#pragma once
#ifndef NG_NET_ETHER_H
#define NG_NET_ETHER_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>

#define ETH_MTU 1536

struct _packed mac_addr {
        uint8_t data[6];
};

enum ethertype {
        ETH_IP = 0x0800,
        ETH_ARP = 0x0806,
};

struct _packed eth_hdr {
        struct mac_addr dst_mac;
        struct mac_addr src_mac;
        uint16_t ethertype;
        uint8_t data[];
};

size_t print_mac_addr(struct mac_addr mac);

size_t make_eth_hdr(void *buf, struct mac_addr dst, struct mac_addr src,
                    uint16_t ethertype);

#endif // NG_NET_ETHER_H

