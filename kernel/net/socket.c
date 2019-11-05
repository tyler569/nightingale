
#include <basic.h>
#include <ng/malloc.h>
#include <ng/panic.h>
#include <ng/syscall.h>
#include <ng/syscalls.h>
#include <ng/thread.h>
#include <ng/fs.h>
#include <ng/net/socket.h>
#include <ng/net/ether.h>
#include <ng/net/ip.h>
#include <ng/net/udp.h>
#include <ng/net/net_if.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

struct dmgr sockets = {0};

uint64_t flow_hash(uint32_t myip, uint32_t othrip, uint16_t myport,
                   uint16_t othrport) {
        uint64_t r = 0xDEAFBEAD1234L;
        r *= myip * 7;
        r ^= r << 17;
        r *= othrip * 13;
        r ^= r >> 1;
        r *= myport * 5;
        r ^= r << 7;
        r *= othrport;
        return r;
}

struct datagram {
        size_t len;
        char data[];
};

struct socket_extra {
        int af_type;
        int sock_type;
        int protocol;

        struct net_if *intf;

        uint16_t my_port;
        uint16_t othr_port;
        uint32_t my_ip;
        uint32_t othr_ip;

        uint64_t flow_hash;

        struct list datagrams;
};

enum socket_flags {
        SOCKET_MATCH_SRC,
        SOCKET_MATCH_BOTH,
};

struct packet_info {
        struct ip_hdr *packet;
        uint64_t flow_hash;
        struct fs_node *found;
};

struct net_if *global_hack_nic = NULL;

void sockets_init(struct net_if *nic) {
        dmgr_init(&sockets);
        global_hack_nic = nic;
}

void dispatch_to_socket_if_match(void *_s, void *_i) {
        struct fs_node *socket = _s;
        struct packet_info *info = _i;

        struct ip_hdr *ip = info->packet;
        struct udp_pkt *udp = (struct udp_pkt *)&ip->data;

        struct socket_extra *se = socket->memory;

        if (info->found)  return;

        // match that packet is destined for this (ip, port)
        if (!(
                se->my_port == ntohs(udp->dst_port) &&
                se->my_ip   == ntohl(ip->dst_ip)
        )) {
                return;
        }

        // match source if (other) set in socket
        if (se->othr_port && !(
                se->othr_port == ntohs(udp->src_port) &&
                se->othr_ip   == ntohl(ip->src_ip)
        )) {
                return;
        }

        size_t len = ntohs(udp->len) - 8;

        struct datagram *datagram = malloc(sizeof(struct datagram) + len);
        memcpy(datagram->data, udp->data, len);
        datagram->len = len;

        list_append(&se->datagrams, datagram);

        info->found = socket;
}

void socket_dispatch(struct ip_hdr *ip) {
        struct udp_pkt *udp = (struct udp_pkt *)ip->data;
        uint64_t hash = flow_hash(ntohl(ip->dst_ip), ntohl(ip->src_ip),
                                  ntohs(udp->dst_port), ntohs(udp->src_port));

        size_t len = ntohs(udp->len) - 8;
        struct packet_info p = {
                .packet = ip,
                .flow_hash = hash,
        };

        dmgr_foreachp(&sockets, dispatch_to_socket_if_match, &p);

        if (p.found) {
                wake_blocked_threads(&p.found->blocked_threads);
        } else {
                // undelivered
                // do anything?
        }
}

#define SOCKET_CHECK_BOILER \
        struct fs_node *node = ofd->node; \
        assert(node->filetype == NET_SOCK); \
        struct socket_extra *se = node->memory; \
        assert(se->af_type == AF_INET); \
        assert(se->sock_type == SOCK_DGRAM); \
        assert(se->protocol == IPPROTO_UDP);


ssize_t socket_read(struct open_fd *ofd, void *data, size_t len) {
        SOCKET_CHECK_BOILER

        struct datagram *dg = list_pop_front(&se->datagrams);
        if (!dg)  return -1;

        size_t count = min(dg->len, len);
        memcpy(data, dg->data, count);

        free(dg);

        return count;
}

ssize_t socket_write(struct open_fd *ofd, const void *data, size_t len) {
        SOCKET_CHECK_BOILER

        // CHEATS
        // BAD
        // FIX
        // TODO: ARP system
        struct mac_addr gw_mac = {{0x52, 0x55, 0x0a, 0x00, 0x02, 0x02}};
        struct mac_addr zero_mac = {{0, 0, 0, 0, 0, 0}};

        size_t ix = 0;
        uint8_t *packet = zmalloc(ETH_MTU);

        ix = make_eth_hdr(packet, gw_mac, zero_mac, ETH_IP);
        struct ip_hdr *ip = (struct ip_hdr *)((char *)packet + ix);
        ix += make_ip_hdr(packet + ix, 0x4050, IPPROTO_UDP, se->othr_ip);
        struct udp_pkt *udp = (struct udp_pkt *)((char *)packet + ix);
        ix += make_udp_hdr(packet + ix, se->my_port, se->othr_port);

        memcpy(packet + ix, data, len);

        ix += len;
        ip->total_len = htons(ix - sizeof(struct eth_hdr));
        udp->len = htons(len + sizeof(struct udp_pkt));
        place_ip_checksum((struct ip_hdr *)(packet + sizeof(struct eth_hdr)));

        send_packet(se->intf, packet, ix);
        free(packet);

        return len; // check for MTU later
}

struct syscall_ret sys_socket(int domain, int type, int protocol) {
        if (domain != AF_INET)  return error(EAFNOSUPPORT);
        if (type != SOCK_DGRAM)  return error(EINVAL);
        if (protocol != IPPROTO_UDP)  return error(EPROTONOSUPPORT);

        struct socket_extra *se = zmalloc(sizeof(struct socket_extra));

        se->af_type = domain;
        se->sock_type = type;
        se->protocol = protocol;
        se->intf = NULL; // set at bind() I think

        struct fs_node *new_sock = zmalloc(sizeof(struct fs_node));

        new_sock->filetype = NET_SOCK;
        new_sock->permission = USR_READ | USR_WRITE;
        new_sock->ops.read = socket_read;
        new_sock->ops.write = socket_write;
        new_sock->memory = se;

        dmgr_insert(&sockets, new_sock);

        struct open_fd *new_ofd = zmalloc(sizeof(struct open_fd));
        new_ofd->node = new_sock;
        new_ofd->flags = USR_READ | USR_WRITE;

        int fd = dmgr_insert(&running_process->fds, new_ofd);

        return value(fd);
}

struct syscall_ret sys_bind(int sockfd, struct sockaddr *_addr,
                            socklen_t addrlen) {
        if (addrlen != 16)  return error(EINVAL);

        struct open_fd *ofd = dmgr_get(&running_process->fds, sockfd);
        if (!ofd)  return error(EBADF);

        struct fs_node *sock = ofd->node;
        if (sock->filetype != NET_SOCK)  return error(EINVAL);

        struct socket_extra *extra = sock->memory;

        // something something check to make sure the address isn't already in
        // use

        struct sockaddr_in *addr = (void *)_addr;
        extra->my_ip = addr->sin_addr.s_addr;
        extra->my_port = addr->sin_port;
        extra->intf = global_hack_nic;  // static, passed to init

        return value(0);
}

struct syscall_ret sys_connect(int sockfd, struct sockaddr *_addr,
                               socklen_t addrlen) {
        if (addrlen != 16)  return error(EINVAL);

        struct open_fd *ofd = dmgr_get(&running_process->fds, sockfd);
        if (!ofd)  return error(EBADF);

        struct fs_node *sock = ofd->node;
        if (sock->filetype != NET_SOCK)  return error(EINVAL);

        struct socket_extra *extra = sock->memory;

        // something something different behavior for SOCK_STREAM
        // TCP does a lot here

        struct sockaddr_in *addr = (void *)_addr;
        extra->othr_port = addr->sin_port;
        extra->othr_ip = addr->sin_addr.s_addr;
        extra->flow_hash = flow_hash(extra->my_ip, extra->othr_ip,
                                     extra->my_port, extra->othr_port);

        return value(0);
}

struct syscall_ret sys_send(int sockfd, const void *buf, size_t len,
                            int flags) {
        struct open_fd *ofd = dmgr_get(&running_process->fds, sockfd);
        if (!ofd)  return error(EBADF);

        struct fs_node *sock = ofd->node;
        if (sock->filetype != NET_SOCK)  return error(EINVAL);

        struct socket_extra *extra = sock->memory;

        // send is just write if the flags are 0
        // I don't support non-0 flags.
        if (flags)  return error(ETODO);

        int written = socket_write(ofd, buf, len);
        return value(written);
}

struct syscall_ret sys_recv(int sockfd, void *buf, size_t len, int flags) {
        struct open_fd *ofd = dmgr_get(&running_process->fds, sockfd);
        if (!ofd)  return error(EBADF);

        struct fs_node *sock = ofd->node;
        if (sock->filetype != NET_SOCK)  return error(EINVAL);

        struct socket_extra *extra = sock->memory;

        // recv is just write if the flags are 0
        // I don't support non-0 flags.
        if (flags)  return error(ETODO);

        ssize_t read_len;
        while ((read_len = socket_read(ofd, buf, len)) == -1) {
                block_thread(&sock->blocked_threads);
        }
        return value(read_len);
}

struct syscall_ret sys_sendto(int sockfd, const void *buf, size_t len,
                              int flags, const struct sockaddr *addr,
                              size_t addrlen) {
        return error(ETODO);
}


struct syscall_ret sys_recvfrom(int sockfd, void *buf, size_t len, int flags,
                                struct sockaddr *addr, size_t *addrlen) {
        return error(ETODO);
}
