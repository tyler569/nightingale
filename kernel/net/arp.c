
#include <ng/basic.h>
#include <ng/net/arp.h>
#include <ng/print.h>
#include <stddef.h>
#include <stdint.h>
#include <ng/net/ether.h>
#include <ng/net/inet.h>
#include <ng/net/ip.h>

static struct mac_addr bcast_mac = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
static struct mac_addr zero_mac = {{0, 0, 0, 0, 0, 0}};
static uint32_t my_ip = 0x0a00020f; // 10.0.2.15

size_t make_ip_arp_req(void *buf, struct mac_addr my_mac, uint32_t my_ip,
                       uint32_t req_ip) {
        struct eth_hdr *req = buf;
        size_t loc = make_eth_hdr(req, bcast_mac, zero_mac, 0x0806);

        struct arp_pkt *arp = (void *)&req->data;
        arp->hw_type = htons(1);    // Ethernet
        arp->proto = htons(0x0800); // IP
        arp->hw_size = 6;
        arp->proto_size = 4;
        arp->op = htons(ARP_REQ);
        arp->sender_mac = my_mac;
        arp->sender_ip = htonl(my_ip);
        arp->target_mac = zero_mac;
        arp->target_ip = htonl(req_ip);

        return loc + sizeof(*arp);
}

size_t make_ip_arp_resp(void *buf, struct mac_addr my_mac,
                        struct arp_pkt *req) {
        struct eth_hdr *resp = buf;

        size_t loc = make_eth_hdr(resp, req->sender_mac, my_mac, 0x0806);

        struct arp_pkt *arp = (void *)&resp->data;
        arp->hw_type = htons(1);    // Ethernet
        arp->proto = htons(0x0800); // IP
        arp->hw_size = 6;
        arp->proto_size = 4;
        arp->op = htons(ARP_RESP);
        arp->sender_mac = my_mac;
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
