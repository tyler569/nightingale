
#include <basic.h>
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <panic.h>
#include <fs/vfs.h>
#include <net/net_if.h>
#include <net/ether.h>
#include <net/ip.h>
#include <net/udp.h>
// #include "socket.h"

enum sock_type {
    SOCK_DGRAM,
};

enum af_type {
    AF_INET,
};

enum ipproto {
    SOCK_DEFAULT,
    SOCK_UDP,
};

uint64_t flow_hash(uint32_t myip, uint32_t othrip, uint16_t myport, uint16_t othrport) {
    uint64_t r = 103;
    r *= myip * 7;
    r *= othrip * 13;
    r *= myport * 5;
    r /= othrport;
    return r;
}

struct pkt_queue {
    struct pkt_queue *next;
    size_t len;
    char data[0];
};

struct socket {
    struct fs_node node;

    int af_type;
    int sock_type;
    int protocol;

    struct net_if *intf;

    uint16_t my_port;
    uint16_t othr_port;
    uint32_t my_ip;
    uint32_t othr_ip;
    uint64_t flow_hash;
};

struct sock_dgram {
    struct socket sock;
    struct pkt_queue *first;
    struct pkt_queue *last;
};

struct sock_stream {
    struct socket sock;
    struct buf *buffer;
};

ssize_t socket_read(struct fs_node *sock_node, void *data, size_t len) {

    struct socket *sock = (void *)sock_node;

    if (sock->af_type != AF_INET) {
        panic("Unsupported AF_TYPE: %i\n", sock->af_type);
    }
    if (sock->sock_type != SOCK_DGRAM) {
        panic("Unsupported SOCK_TYPE: %i\n", sock->sock_type);
    }
    if (sock->protocol != SOCK_DEFAULT || sock->protocol != PROTO_UDP) {
        panic("Unsupported protocol: %i\n", sock->protocol);
    }

    struct sock_dgram *dg = (void *)sock; // TEMP

    while (!dg->first) {} // block until there is a packet

    size_t count = min(dg->first->len, len);
    memcpy(data, dg->first->data, count);

    // If that's NULL, then that's NULL...
    // It is what it is
    dg->first = dg->first->next;

    // Something something free(dg->first)
    // Something something dg->last
    // TODO ^

    return count;
}

ssize_t socket_write(struct fs_node *sock_node, const void *data, size_t len) {

    struct socket *sock = (void *)sock_node;

    // CHEATS
    // BAD
    // FIX
    // TODO: ARP system
    struct mac_addr gw_mac = {{ 0x52, 0x55, 0x0a, 0x00, 0x02, 0x02 }};
    struct mac_addr zero_mac = {{ 0, 0, 0, 0, 0, 0 }};

    if (sock->af_type != AF_INET) {
        panic("Unsupported AF_TYPE: %i\n", sock->af_type);
    }
    if (sock->sock_type != SOCK_DGRAM) {
        panic("Unsupported SOCK_TYPE: %i\n", sock->sock_type);
    }
    if (sock->protocol != SOCK_DEFAULT || sock->protocol != PROTO_UDP) {
        panic("Unsupported protocol: %i\n", sock->protocol);
    }

    struct sock_dgram *dg = (void *)sock; // TEMP

    size_t ix = 0;
    uint8_t *packet = calloc(ETH_MTU, 1);

    ix = make_eth_hdr(packet, gw_mac, zero_mac, ETH_IP);
    ix += make_ip_hdr(packet + ix, 0x4050, PROTO_UDP, sock->othr_ip);
    ix += make_udp_hdr(packet + ix, sock->my_port, sock->othr_port);
    memcpy(packet + ix, data, len);
    ix += len;
    place_ip_checksum(packet);
    send_packet(sock->intf, packet, ix);
    free(packet);
}

