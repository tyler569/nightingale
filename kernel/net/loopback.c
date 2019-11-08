
#include <basic.h>
#include <ng/net/network.h>
#include <ng/net/net_if.h>
#include <ng/net/loopback.h>
#include <nc/stdio.h>

void loopback_send_packet(struct net_if *lo, void *data, size_t len) {
        printf("loopback!");
        dispatch_packet(data, len, lo);
}

struct net_if loopback = {
    .type = LOOPBACK,
    .id = -1,
    .mac_addr = {{ 0x00, 0x01, 0x01, 0x33, 0x44, 0x55 }},
    .ip_addr = 0x7F000001, /* 127.0.0.1 */
    .loopback = {0},
    .send_packet = loopback_send_packet,
};

