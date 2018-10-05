
#include <basic.h>
#include <debug.h>
#include <print.h>
#include <queue.h>
#include <thread.h>
#include "net_if.h"
#include "inet.h"
#include "ether.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"
#include "udp.h"
#include "tcp.h"
#include "knetworker.h"

struct queue incoming_packets = {0};

// This is a queue to be consistent with other places that threads block
// this can definitely never have more than the one thread in it.
struct queue knetworker_block = {0};

void arp_incoming(struct arp_pkt* arp, struct net_if* iface);
void ip_incoming(struct ip_hdr* ip, struct net_if* iface);
void icmp_incoming(struct icmp_pkt* icmp, struct net_if* iface);
void udp_incoming(struct udp_pkt* udp, struct net_if* iface);
void tcp_incoming(struct tcp_pkt* tcp, struct net_if* iface);

void knetworker(void) {
    while (true) {
        if (!incoming_packets.head) {
            block_thread(&knetworker_block);
        }

        struct queue_object* qo = queue_dequeue(&incoming_packets);

        // check if the packet is ARP or IP (or other)
        // ARP -> arp_incoming
        // IP ->  ip_incoming
        //   check if to our IP address
        //   check if TCP or UDP
        //     check if dest port in use
        //     -> udp_incoming / tcp_incoming
        
        struct pkt_desc* pkt = (struct pkt_desc*)&qo->data;
        struct eth_hdr* eth = (struct eth_hdr*)&pkt->data;
        struct net_if* iface = pkt->iface;

        switch (ntohs(eth->ethertype)) {
        case ETH_ARP:
            arp_incoming((struct arp_pkt*)&eth->data, iface);
            break;
        case ETH_IP:
            ip_incoming((struct ip_hdr*)&eth->data, iface);
            break;
        default:
            printf("unknown ethertype (%#06hx)\n", ntohs(eth->ethertype));
        }
    }
}

void arp_incoming(struct arp_pkt* arp, struct net_if* iface) {
    if (ntohs(arp->op) == ARP_REQ) {
        void* resp = calloc(ETH_MTU, 1);
        size_t len = make_ip_arp_resp(resp, iface->mac_addr, arp);
        iface->send_packet(iface, resp, len);
    } else {
        // update a cache table or something
    }
}

void ip_incoming(struct ip_hdr* ip, struct net_if* iface) {
    if (ip->hdr_len != 4)
        goto drop;
    if (ip->version != 4)
        goto drop;

    // check if to my ip ?

    switch (ip->proto) {
    case PROTO_ICMP:
        // icmp_incoming((struct icmp_pkt*)&ip->data, iface);
        break;
    case PROTO_UDP:
        udp_incoming((struct udp_pkt*)&ip->data, iface);
        break;
    case PROTO_TCP:
        tcp_incoming((struct tcp_pkt*)&ip->data, iface);
        break;
    default:
        printf("unknown ip protocol number (%hhi)\n", ip->proto);
    }

    drop:
    return;
}

void tcp_incoming(struct tcp_pkt* tcp, struct net_if* iface) {}
void udp_incoming(struct udp_pkt* udp, struct net_if* iface) {}

