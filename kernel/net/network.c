
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
        uint32_t device_addr = pci_find_device_by_id(0x10ec, 0x8139);
        if (device_addr == -1) {
                printf("network: no nic found\n");
                return;
        }

        //struct net_if *nic = init_rtl8139(device_addr, 0x0a00020f /* 10.0.2.15 */);
        struct net_if *nic = init_rtl8139(device_addr, 0x0a32010f /* 10.50.1.15 */);
        sockets_init(nic);
}

// dispatch to sockets
void dispatch_packet(void *packet, size_t _len, struct net_if *intf) {
        struct eth_hdr *eth = packet;

        if (eth->ethertype == htons(ETH_ARP)) {
                void *resp = zmalloc(ETH_MTU);
                struct arp_pkt *arp = (struct arp_pkt *)&eth->data;

                size_t len = make_ip_arp_resp(resp, intf->mac_addr, arp);
                send_packet(intf, resp, len);

                free(resp);
        }

        // TODO: ICMP

        if (eth->ethertype == htons(ETH_IP)) {
                struct ip_hdr *ip = (struct ip_hdr *)&eth->data;
                if (ip->proto == IPPROTO_UDP)
                        socket_dispatch(ip);
        }
}

// send to network
void send_packet(struct net_if *nic, void *data, size_t len) {
        struct eth_hdr *eth = data;
        eth->source_mac = nic->mac_addr;

        if (ntohs(eth->ethertype) == ETH_IP) {
                struct ip_hdr *ip = (struct ip_hdr *)&eth->data;
                if (ip->source_ip == 0) {
                        ip->source_ip = htonl(nic->ip_addr);
                }
        }

        nic->send_packet(nic, data, len);
}

