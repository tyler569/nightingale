
#include <basic.h>
#include <ng/net/ether.h>
#include <ng/print.h>
#include <stddef.h>
#include <stdint.h>
#include <ng/net/inet.h>

size_t print_mac_addr(struct mac_addr mac) {
        return printf("%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", mac.data[0],
                      mac.data[1], mac.data[2], mac.data[3], mac.data[4],
                      mac.data[5]);
}

size_t make_eth_hdr(void *buf, struct mac_addr destination, struct mac_addr source,
                    uint16_t type) {
        struct eth_hdr *pkt = buf;

        pkt->destination_mac = destination;
        pkt->source_mac = source;
        pkt->ethertype = htons(type);
        return sizeof(struct eth_hdr);
}

