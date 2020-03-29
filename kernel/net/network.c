
#include <basic.h>
#include <nc/stdio.h>
#include <ng/pci.h>
#include <ng/net/socket.h>
#include <ng/net/ip.h>
#include <ng/net/arp.h>
#include <ng/net/udp.h>
#include <ng/net/ether.h>
#include <ng/net/net_if.h>
#include <ng/drv/rtl8139.h>
#include <ng/net/network.h>

// TODO:
// fix names - "send" vs "dispatch" packet
// ugh

void network_init(void) {
        pci_device_callback(0x10ec, 0x8139, rtl8139_init);
}

// dispatch to sockets
void dispatch_packet(void *packet, size_t _len, struct net_if *intf) {
#if 0
        struct eth_hdr *eth = packet;

        if (eth->ethertype == htons(ETH_ARP)) {
                void *resp = zmalloc(ETH_MTU);
                struct arp_pkt *arp = (struct arp_pkt *)&eth->data;

                size_t len = make_ip_arp_resp(resp, intf->mac_addr, arp);
                send_packet(intf, resp, len);

                free(resp);
        }

        // TODO: replace all of this with ipstack

        if (eth->ethertype == htons(ETH_IP)) {
                struct ip_hdr *ip = (struct ip_hdr *)&eth->data;
                if (ip->proto == IPPROTO_UDP)
                        socket_dispatch(ip);
        }
#endif
}

// send to network
void send_packet(struct net_if *nic, void *data, size_t len) {
#if 0
        struct eth_hdr *eth = data;
        eth->source_mac = nic->mac_addr;

        if (ntohs(eth->ethertype) == ETH_IP) {
                struct ip_hdr *ip = (struct ip_hdr *)&eth->data;
                if (ip->source_ip == 0) {
                        ip->source_ip = htonl(nic->ip_addr);
                        place_ip_checksum(ip);
                }
        }

        nic->send_packet(nic, data, len);
#endif
}

