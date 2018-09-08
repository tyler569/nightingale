
#include <basic.h>
#include <stdint.h>
#include <stddef.h>
#include "ether.h"
#include "inet.h"
#include "ip.h"
#include "udp.h"

size_t make_udp_hdr(void *buf, uint16_t src_port, uint16_t dst_port) {
    struct udp_pkt *udp = buf;

    udp->src_port = htons(src_port);
    udp->dst_port = htons(dst_port);
    udp->len = 0; // placed after
    udp->checksum = 0; // checksum disabled;

    return sizeof(*udp);
}

