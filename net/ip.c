
#include <ng/basic.h>
#include <net/ip.h>
#include <ng/print.h>
#include <stddef.h>
#include <stdint.h>
#include <net/ether.h>
#include <net/inet.h>

static uint32_t my_ip = 0x0a00020f; // TODO: TMP

void print_ip_addr(uint32_t ip) {
        printf("%i.%i.%i.%i", (ip >> 24) & 0xff, (ip >> 16) & 0xff,
               (ip >> 8) & 0xff, ip & 0xff);
}

void place_ip_checksum(struct ip_hdr *ip) {
        uint16_t *ip_chunks = (uint16_t *)ip;
        uint32_t checksum32 = 0;
        for (int i = 0; i < ip->hdr_len * 2; i += 1) {
                checksum32 += ip_chunks[i];
        }
        uint16_t checksum = (checksum32 & 0xFFFF) + (checksum32 >> 16);

        ip->hdr_checksum = ~checksum;
}

size_t make_ip_hdr(void *buf, uint16_t id, uint8_t proto, uint32_t dst_ip) {
        struct ip_hdr *ip = buf;

        ip->version = 4;
        ip->hdr_len = 5;
        ip->dscp = 0;
        ip->total_len = 0; // get later!
        ip->id = htons(id);
        ip->flags_frag = htons(0x4000); // dnf
        ip->ttl = 255;
        ip->proto = proto;
        ip->hdr_checksum = 0; // get later!
        ip->src_ip = htonl(my_ip);
        ip->dst_ip = htonl(dst_ip);

        return sizeof(*ip);
}
