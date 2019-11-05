
#pragma once
#ifndef NG_NET_IP_H
#define NG_NET_IP_H

#include <basic.h>
#include <nc/sys/socket.h>
#include <stddef.h>
#include <stdint.h>

struct _packed ip_hdr {
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

void print_ip_addr(uint32_t ip);
void place_ip_checksum(struct ip_hdr *ip);
size_t make_ip_hdr(void *buf, uint16_t id, uint8_t proto, uint32_t dst_ip);

#endif // NG_NET_IP_H

