
#pragma once
#ifndef NIGHTINGALE_NET_ETHER_H
#define NIGHTINGALE_NET_ETHER_H

#include <basic.h>
#include <stdint.h>
#include <stddef.h>

#define be16_t uint16_t

struct __packed mac_addr {
    uint8_t data[8];
};

enum ethertype {
    ETH_IP = 0x0800,
    ETH_ARP = 0x0806,
};

struct __packed eth_hdr {
    struct mac_addr dst_mac;
    struct mac_addr src_mac;
    be16_t ethertype;
    uint8_t data[0];
};

size_t print_mac_addr(struct mac_addr mac);

size_t make_eth_hdr(void *buf, struct mac_addr dst,
                    struct mac_addr src, uint16_t ethertype);

#endif
