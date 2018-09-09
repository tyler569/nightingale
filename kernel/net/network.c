
#include <basic.h>
#include <stdint.h>

#include <malloc.h>
#include <print.h>
#include <pci.h>

#include "arp.h"
#include "inet.h"
#include "ether.h"
#include "socket.h"
#include "rtl8139.h"

void network_init(void) {

    // 
    // find a network card
    // init it
    // init sockets with that card
    //
    
    uint32_t rtl_addr = pci_find_device_by_id(0x10ec, 0x8139);
    if (rtl_addr == 0) {
        printf("network: init failed, no nic found\n");
        return;
    }
    struct net_if* rtl = init_rtl8139(rtl_addr);

    sockets_init(rtl);
}

void dispatch_packet(void* pkt, size_t len, struct net_if* iface) {
    struct eth_hdr* eth = pkt;

    // printf("Received a ethertype: %#06hx\n", ntohs(eth->ethertype));

    // If arp respond
    if (eth->ethertype == htons(ETH_ARP)) {
        void* resp = calloc(ETH_MTU, 1);
        size_t len = make_ip_arp_resp(resp, iface->mac_addr, (struct arp_pkt*)&eth->data);
        iface->send_packet(iface, resp, len);
    }

    // if icmp and echo req respond
    // TODO

    // if tcp/udp send to sockets
    if (eth->ethertype == htons(ETH_IP)) {
        struct ip_hdr* ip = (void*)&eth->data;
        if (ip->proto == PROTO_UDP) {
            socket_dispatch(ip);
        }
    }
}

