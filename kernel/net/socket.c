
#include <basic.h>
#include <stdint.h>
#include <stddef.h>
#include <fs/vfs.h>
#include "socket.h"

enum sock_type {
    SOCK_DGRAM;
};

enum af_type {
    AF_INET;
};

uint64_t flow_hash(uint32_t myip, uint32_t othrip, uint16_t myport, uint16_t othrport) {
    uint64_t r = 103;
    r *= myip * 7;
    r *= othrip * 13;
    r *= myport * 5;
    r /= othrport;
    return r;
}

struct socket {
    struct fs_node node;
    struct buf buffer;

    uint16_t my_port;
    uint16_t othr_port;
    uint32_t my_ip;
    uint32_t othr_ip;

    uint64_t flow_hash;
};

ssize_t socket_read(struct socket *sock, void *data, size_t len) {
}

ssize_t socket_write(struct socket *sock, const void *data, size_t len) {
}
