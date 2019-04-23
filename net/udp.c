
#include <ng/basic.h>
#include "udp.h"
#include <stddef.h>
#include <stdint.h>
#include "ether.h"
#include "inet.h"
#include "ip.h"

size_t make_udp_hdr(void *buf, uint16_t src_port, uint16_t dst_port) {
        struct udp_pkt *udp = buf;

        udp->src_port = htons(src_port);
        udp->dst_port = htons(dst_port);
        udp->len = 0;      // placed after
        udp->checksum = 0; // checksum disabled;

        return sizeof(*udp);
}

// non-solidified:

uintptr_t udp_new_socket() {
        // called by sys_socket and does protocol-specific things
        // returns handle into socket_table
        // sys_socket can then put that into the fs_node_table
        return 0;
}

struct udp_extra; // maybe ?

void udp_bind(struct udp_extra *u) {
        // called by sys_bind and does protocol-specific things
        // in the case of UDP, this is probably almost a NOP,
        // but may be needed for things like TCP
}

void udp_connect() {
        // called by sys_connect and does protocol-specific things;
        // in the case of UDP, this is probably almost a NOP,
        // but may be needed for things like TCP
}

void udp_send() {
        // ?
}

void udp_recv() {
        // ?
}

/*
 * Can these things be function pointers?
 * maybe there's a structure of all the supported protocols
 * with pointers to the implementations of their operations
 */
