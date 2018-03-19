
#include <basic.h>
#include <stdint.h>
#include <stddef.h>
#include <print.h>
#include "ether.h"
#include "ip.h"
#include "inet.h"
#include "arp.h"

static struct mac_addr my_mac, bcast_mac, zero_mac; // TODO TEMP
static char *my_ip;
uint32_t ip_from_str(char *ip_str);

size_t make_ip_arp_req(void *buf, char *my_ip, char *req_ip) {
    struct eth_hdr *req = buf;
    size_t loc = make_eth_hdr(req, bcast_mac, my_mac, 0x0806);

    struct arp_pkt *arp = (void *)&req->data;
    arp->hw_type = htons(1); // Ethernet
    arp->proto = htons(0x0800); // IP
    arp->hw_size = 6;
    arp->proto_size = 4;
    arp->op = htons(ARP_REQ);
    arp->sender_mac = my_mac; // global - consider parametrizing this
    arp->sender_ip = htonl(ip_from_str(my_ip));
    arp->target_mac = zero_mac;
    arp->target_ip = htonl(ip_from_str(req_ip));

    return loc + sizeof(*arp);
}

size_t make_ip_arp_resp(void *buf, struct arp_pkt *req) {
    struct eth_hdr *resp = buf;

    size_t loc = make_eth_hdr(resp, req->sender_mac, my_mac, 0x0806);

    struct arp_pkt *arp = (void *)&resp->data;
    arp->hw_type = htons(1); // Ethernet
    arp->proto = htons(0x0800); // IP
    arp->hw_size = 6;
    arp->proto_size = 4;
    arp->op = htons(ARP_RESP);
    arp->sender_mac = my_mac; // global - consider parametrizing this
    arp->sender_ip = htonl(my_ip);
    arp->target_mac = req->sender_mac;
    arp->target_ip = req->sender_ip;

    return loc + sizeof(*arp);
}


void print_arp_pkt(struct arp_pkt *arp) {
    int op = ntohs(arp->op);
    if (op == ARP_REQ) {
        printf("ARP Request who-has ");
        print_ip_addr(ntohl(arp->target_ip));
        printf(" tell ");
        print_ip_addr(ntohl(arp->sender_ip));
        printf("\n");
    } else if (op == ARP_RESP) {
        printf("ARP Responce ");
        print_ip_addr(ntohl(arp->sender_ip));
        printf(" is-at ");
        print_mac_addr(arp->sender_mac);
        printf("\n");
    } else {
        printf("Unrecognised ARP OP: %i\n", ntohs(arp->op));
    }
}


