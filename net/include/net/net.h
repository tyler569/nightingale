
#ifndef IPSTACK_NET_H
#define IPSTACK_NET_H 1

#include <basic.h>
#include <net/core.h>
#include <net/if.h>
#include <netinet/in.h>

enum ethertype {
    ETH_IP = 0x0800,
    ETH_ARP = 0x0806,
};

struct __PACKED ethernet_header {
    struct mac_address destination_mac;
    struct mac_address source_mac;
    be16 ethertype;
    char data[];
};

struct __PACKED arp_header {
    // eth_hdr
    be16 hw_type;
    be16 proto;
    uint8_t hw_size;
    uint8_t proto_size;
    be16 op;
    struct mac_address sender_mac;
    be32 sender_ip;
    struct mac_address target_mac;
    be32 target_ip;
};

struct __PACKED ip_header {
    // eth_hdr
    uint8_t header_length : 4;
    uint8_t version : 4;
    uint8_t dscp;
    be16 total_length;
    be16 id;
    be16 flags_frag;
    uint8_t ttl;
    uint8_t proto;
    be16 header_checksum;
    be32 source_ip;
    be32 destination_ip;
    char data[];
};

enum icmp_type {
    ICMP_ECHO_REQ = 8,
    ICMP_ECHO_RESP = 0,
};

struct __PACKED icmp_header {
    // ip_hdr
    uint8_t type;
    uint8_t code;
    be16 checksum;
    be16 ident;
    be16 sequence;
    be32 timestamp;
    be32 timestamp_low;
    char data[];
};

struct __PACKED udp_header {
    // ip_hdr
    be16 source_port;
    be16 destination_port;
    be16 length;
    be16 checksum;
    char data[];
};

enum tcp_flags {
    TCP_NONE = 0,
    TCP_URG = 1 << 0,
    TCP_ACK = 1 << 1,
    TCP_PSH = 1 << 2,
    TCP_RST = 1 << 3,
    TCP_SYN = 1 << 4,
    TCP_FIN = 1 << 5,
};

struct __PACKED tcp_header {
    // ip hdr
    be16 source_port;
    be16 destination_port;
    be32 seq;
    be32 ack;
    be16 _reserved : 4;
    be16 offset : 4;
    be16 f_fin : 1;
    be16 f_syn : 1;
    be16 f_rst : 1;
    be16 f_psh : 1;
    be16 f_ack : 1;
    be16 f_urg : 1;
    be16 _reserved2 : 2;
    be16 window;
    be16 checksum;
    be16 urg_ptr;
    char data[];
};

typedef struct mac_address mac_address;

enum arp_op {
    ARP_REQ = 1,
    ARP_RESP = 2,
};

static const struct mac_address broadcast_mac = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
static const struct mac_address zero_mac = {{0, 0, 0, 0, 0, 0}};

struct route {
    be32 prefix;
    be32 netmask;
    be32 next_hop;
};

struct pending_mac_query {
    be32 ip;
    struct mac_address mac;

    int attempts;

    list pending_pks;

    list_node queries; // net_device.pending_mac_queries
};

struct mac_address mac_from_str_trad(char *mac_str);
struct mac_address mac_from_str(char *mac_str);
void print_mac_address(struct mac_address mac);
bool mac_eq(struct mac_address a, struct mac_address b);

uint32_t ip_from_str(char *ip_str);
void print_ip_address(be32 ip);
void print_arp_pkt(struct pkb *pk);

struct ethernet_header *eth_hdr(struct pkb *pk);
struct arp_header *arp_hdr(struct pkb *pk);
struct ip_header *ip_hdr(struct pkb *pk);
struct udp_header *udp_hdr(struct ip_header *ip);
struct tcp_header *tcp_hdr(struct ip_header *ip);
struct icmp_header *icmp_hdr(struct ip_header *ip);

int ip_len(struct pkb *pk);
int tcp_len(struct pkb *pk);
int udp_len(struct pkb *pk);

void *tcp_data(struct pkb *pk);
void *udp_data(struct pkb *pk);

void ip_checksum(struct pkb *);
void icmp_checksum(struct pkb *);
void udp_checksum(struct pkb *);
void tcp_checksum(struct pkb *ip);

void process_ethernet(struct pkb *pk);

void query_for(struct net_device *intf, be32 address, struct pkb *pk);
void arp_query(struct pkb *pk, be32 address, struct net_device *intf);
struct mac_address arp_cache_get(struct net_device *intf, be32 ip);
void arp_cache_put(struct net_device *intf, be32 ip, struct mac_address mac);
void arp_reply(struct pkb *resp, struct pkb *pk);
void process_arp_packet(struct pkb *pk);

void process_ip_packet(struct pkb *pk);

void echo_icmp(struct pkb *pk);
void reply_icmp(struct pkb *resp, struct pkb *pk);

void make_udp(struct socket_impl *s, struct pkb *pk,
        struct sockaddr_in *d_addr, const void *data, size_t len);
void make_tcp(struct socket_impl *s, struct pkb *pk,
        int flags, const void *data, size_t len);

void dispatch(struct pkb *pk);
be32 best_route(be32 address);
struct net_device *interface_containing(be32 ip);

#endif // IPSTACK_NET_H

