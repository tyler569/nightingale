
#ifndef NIGHTINGALE_NET_ICMP_H
#define NIGHTINGALE_NET_ICMP_H

#include <ng/basic.h>
#include <stddef.h>
#include <stdint.h>
#include "ether.h"
#include "inet.h"
#include "ip.h"

enum icmp_type {
        ICMP_ECHO_REQ = 8,
        ICMP_ECHO_RESP = 0,
};

struct __packed icmp_pkt {
        // ip_hdr
        uint8_t type;
        uint8_t code;
        uint16_t checksum;
        uint16_t ident;
        uint16_t sequence;
        //    uint32_t timestamp;
        //    uint32_t timestamp_low;
        uint8_t data[];
};

size_t make_icmp_req(void *buf, int id, int seq);

void place_icmp_checksum(struct icmp_pkt *icmp, size_t extra_len);

#endif
