
#pragma once
#ifndef NIGHTINGALE_NET_IP_H
#define NIGHTINGALE_NET_IP_H

#include <basic.h>
#include <stdint.h>
#include <stddef.h>
#include "ether.h"
#include "inet.h"

struct __packed ip_hdr {
    // eth_hdr
    uint8_t hdr_len : 4;
    uint8_t version : 4;
    uint8_t dscp;
    uint16_t total_len;
    uint16_t id;
    uint16_t flags_frag;
    uint8_t ttl;
    uint8_t proto;
    uint16_t hdr_checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint8_t data[];
};

enum ip_protocol_numbers {
    PROTO_ICMP = 1,
    PROTO_TCP = 6,
    PROTO_UDP = 17,
};

void print_ip_addr(uint32_t ip);
void place_ip_checksum(struct ip_hdr *ip);
size_t make_ip_hdr(void *buf, uint16_t id, uint8_t proto, uint32_t dst_ip);

#endif
