
#pragma once
#ifndef NIGHTINGALE_NET_UDP_H
#define NIGHTINGALE_NET_UDP_H

#include <ng/basic.h>
#include <stdint.h>
#include <stddef.h>
#include "ether.h"
#include "inet.h"
#include "ip.h"

struct __packed udp_pkt {
    // ip_hdr
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t len;
    uint16_t checksum;
    uint8_t data[];
};

size_t make_udp_hdr(void *buf, uint16_t src_port, uint16_t dst_port);

#endif
