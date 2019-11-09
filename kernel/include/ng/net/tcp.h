
#pragma once
#ifndef NG_NET_TCP_H
#define NG_NET_TCP_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include "inet.h"
#include "ip.h"

#define TCP_FLAG_FIN 0x0001
#define TCP_FLAG_SYN 0x0002
#define TCP_FLAG_RST 0x0004
#define TCP_FLAG_PSH 0x0008
#define TCP_FLAG_ACK 0x0010
#define TCP_FLAG_URG 0x0020
#define TCP_FLAG_ECN 0x0040
#define TCP_FLAG_CWR 0x0080
#define TCP_FLAG_NON 0x0100

#define TCP_HDR_LEN_MASK 0xF000

// states

enum { TCP_LISTEN,
       TCP_SYNSENT,
       TCP_SYNRECV,
       TCP_ESTABLISHED,
       TCP_FINWAIT1,
       TCP_FINWAIT2,
       TCP_CLOSEWAIT,
       TCP_CLOSING,
       TCP_LASTACK,
       TCP_TIMEWAIT,
       TCP_CLOSED,
};

struct _packed tcp_pkt {
        // ip_hdr
        uint16_t source_port;
        uint16_t destination_port;
        uint32_t seqeuence_num;
        uint32_t ack_num;
        uint16_t hlen_flags;
        uint16_t window;
        uint16_t checksum;
        uint16_t urgent_ptr;
        // options?
        char data[];
};

#endif // NG_NET_TCP_H

