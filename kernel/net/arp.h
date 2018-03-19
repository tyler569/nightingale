
#pragma once
#ifndef NIGHTINGALE_NET_ARP_H
#define NIGHTINGALE_NET_ARP_H

#include <basic.h>
#include <stdint.h>
#include <stddef.h>

struct __packed arp_pkt {
    // eth_hdr
    uint16_t hw_type;
    uint16_t proto;
    uint8_t hw_size;
    uint8_t proto_size;
    uint16_t op;
    struct mac_addr sender_mac;
    uint32_t sender_ip;
    struct mac_addr target_mac;
    uint32_t target_ip;
};

enum arp_op {
	ARP_REQ = 1,
	ARP_RESP = 2,
};

size_t make_ip_arp_req(void *buf, char *my_ip, char *req_ip);
size_t make_ip_arp_resp(void *buf, struct arp_pkt *req);

void print_arp_pkt(struct arp_pkt *arp);

#endif

