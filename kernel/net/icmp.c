
#include <basic.h>
#include <stdint.h>
#include <stddef.h>
#include <print.h>
#include "ether.h"
#include "inet.h"
#include "ip.h"
#include "icmp.h"

size_t make_icmp_req(void *buf, int id, int seq) {
    struct icmp_pkt *icmp = buf;

    icmp->type = ICMP_ECHO_REQ;
    icmp->code = 0;
    icmp->checksum = 0; // get later!
    icmp->ident = htons(id);
    icmp->sequence = htons(seq);
    icmp->timestamp = 0;
    icmp->timestamp_low = 0;

    return sizeof(*icmp);
}

void place_icmp_checksum(struct icmp_pkt *icmp, size_t extra_len) {
    uint16_t *icmp_chunks = (uint16_t *)icmp;
    uint32_t checksum32 = 0;
    for (int i=0; i<(sizeof(struct icmp_pkt) + extra_len)/2; i+=1) {
        checksum32 += icmp_chunks[i];
    }
    uint16_t checksum = (checksum32 & 0xFFFF) + (checksum32 >> 16);

    icmp->checksum = ~checksum;
}

