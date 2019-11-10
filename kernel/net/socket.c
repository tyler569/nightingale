
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
#include <ng/net/loopback.h>
#include <ng/net/net_if.h>
#include <ng/net/network.h>
#include <nc/errno.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

struct dmgr sockets = {0};

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
        uint16_t other_port;
        uint32_t my_ip;
        uint32_t other_ip;

        uint64_t flow_hash;

        struct list datagrams;
};

enum socket_flags {
        SOCKET_MATCH_SRC,
        SOCKET_MATCH_BOTH,
};

struct packet_info {
        struct ip_hdr *packet;
        struct file *found;
};

struct net_if *global_hack_nic = NULL;
struct net_if *loopback_device = NULL;

extern struct net_if loopback;

void sockets_init(struct net_if *nic) {
        dmgr_init(&sockets);
        global_hack_nic = nic;
        loopback_device = &loopback;
}

void dispatch_to_socket_if_match(void *_s, void *_i) {
        struct file *socket = _s;
        struct packet_info *info = _i;

        struct ip_hdr *ip = info->packet;
        struct udp_pkt *udp = (struct udp_pkt *)&ip->data;

        struct socket_extra *se = socket->memory;

        if (info->found)  return;

        // match that packet is destined for this (port)
        if (!(se->my_port == ntohs(udp->destination_port))) {
                return;
        }

        // match my IP if we'er not bound to 0.0.0.0
        if (se->my_ip && !(se->my_ip == ntohl(ip->destination_ip))) {
                return;
        }

        // match source if (other) set in socket
        if (se->other_port && !(
                se->other_port == ntohs(udp->source_port) &&
                se->other_ip   == ntohl(ip->source_ip)
        )) {
                return; }

        size_t len = ntohs(udp->len) + sizeof(struct ip_hdr);

        struct datagram *datagram = malloc(sizeof(struct datagram) + len);
        memcpy(datagram->data, ip, len);
        datagram->len = len;

        list_append(&se->datagrams, datagram);

        info->found = socket;
}

void socket_dispatch(struct ip_hdr *ip) {
        struct udp_pkt *udp = (struct udp_pkt *)ip->data;

        size_t len = ntohs(udp->len) - 8;
        struct packet_info p = {
                .packet = ip,
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
        struct file *node = ofd->node; \
        assert(node->filetype == FT_SOCKET); \
        struct socket_extra *se = node->memory; \
        assert(se->af_type == AF_INET); \
        assert(se->sock_type == SOCK_DGRAM); \
        assert(se->protocol == IPPROTO_UDP);


ssize_t socket_read(struct open_file *ofd, void *data, size_t len) {
        SOCKET_CHECK_BOILER

        struct datagram *dg = list_pop_front(&se->datagrams);
        if (!dg)  return -1;

        struct ip_hdr *ip = (struct ip_hdr *)dg->data;
        struct udp_pkt *udp = (struct udp_pkt *)&ip->data;

        size_t data_len = udp->len - 8;

        size_t count = min(data_len, len);
        memcpy(data, udp->data, count);

        free(dg);

        return count;
}

struct net_if *do_routing(uint32_t dest_ip) {
        if ((dest_ip & 0xFF000000) == 0x7F000000) {
                return loopback_device;
        } else {
                return global_hack_nic;
        }
}

ssize_t socket_write(struct open_file *ofd, const void *data, size_t len) {
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
        ix += make_ip_hdr(packet + ix, 0x4050, IPPROTO_UDP, se->other_ip, se->my_ip);
        struct udp_pkt *udp = (struct udp_pkt *)((char *)packet + ix); ix += make_udp_hdr(packet + ix, se->my_port, se->other_port);

        memcpy(packet + ix, data, len);

        ix += len;
        ip->total_len = htons(ix - sizeof(struct eth_hdr));
        udp->len = htons(len + sizeof(struct udp_pkt));
        place_ip_checksum((struct ip_hdr *)(packet + sizeof(struct eth_hdr)));

        struct net_if *intf = do_routing(se->other_ip);
        send_packet(intf, packet, ix);

        free(packet);

        return len; // check for MTU later
}

sysret sys_socket(int domain, int type, int protocol) {
        if (domain != AF_INET)  return -EAFNOSUPPORT;
        if (type != SOCK_DGRAM)  return -EINVAL;
        if (protocol != IPPROTO_UDP)  return -EPROTONOSUPPORT;

        struct socket_extra *se = zmalloc(sizeof(struct socket_extra));

        se->af_type = domain;
        se->sock_type = type;
        se->protocol = protocol;

        struct file *new_sock = zmalloc(sizeof(struct file));

        new_sock->filetype = FT_SOCKET;
        new_sock->permissions = USR_READ | USR_WRITE;
        new_sock->read = socket_read;
        new_sock->write = socket_write;
        new_sock->memory = se;

        dmgr_insert(&sockets, new_sock);

        struct open_file *new_ofd = zmalloc(sizeof(struct open_file));
        new_ofd->node = new_sock;
        new_ofd->flags = USR_READ | USR_WRITE;

        int fd = dmgr_insert(&running_process->fds, new_ofd);

        return fd;
}

sysret sys_bind(int sockfd, struct sockaddr *_addr, socklen_t addrlen) {
        if (addrlen != 16)  return -EINVAL;

        struct open_file *ofd = dmgr_get(&running_process->fds, sockfd);
        if (!ofd)  return -EBADF;

        struct file *sock = ofd->node;
        if (sock->filetype != FT_SOCKET)  return -EINVAL;

        struct socket_extra *extra = sock->memory;

        // something something check to make sure the address isn't already in
        // use

        struct sockaddr_in *addr = (void *)_addr;
        extra->my_ip = addr->sin_addr.s_addr;
        extra->my_port = addr->sin_port;

        return 0;
}

sysret sys_connect(int sockfd, struct sockaddr *_addr, socklen_t addrlen) {
        if (addrlen != 16)  return -EINVAL;

        struct open_file *ofd = dmgr_get(&running_process->fds, sockfd);
        if (!ofd)  return -EBADF;

        struct file *sock = ofd->node;
        if (sock->filetype != FT_SOCKET)  return -EINVAL;

        struct socket_extra *extra = sock->memory;

        // something something different behavior for SOCK_STREAM
        // TCP does a lot here

        struct sockaddr_in *addr = (void *)_addr;
        extra->other_port = addr->sin_port;
        extra->other_ip = addr->sin_addr.s_addr;

        return 0;
}

sysret sys_send(int sockfd, const void *buf, size_t len, int flags) {
        struct open_file *ofd = dmgr_get(&running_process->fds, sockfd);
        if (!ofd)  return -EBADF;

        struct file *sock = ofd->node;
        if (sock->filetype != FT_SOCKET)  return -EINVAL;

        struct socket_extra *extra = sock->memory;

        // send is just write if the flags are 0
        // I don't support non-0 flags.
        if (flags)  return -ETODO;

        int written = socket_write(ofd, buf, len);
        return written;
}

sysret sys_recv(int sockfd, void *buf, size_t len, int flags) {
        struct open_file *ofd = dmgr_get(&running_process->fds, sockfd);
        if (!ofd)  return -EBADF;

        struct file *sock = ofd->node;
        if (sock->filetype != FT_SOCKET)  return -EINVAL;

        struct socket_extra *extra = sock->memory;

        // recv is just write if the flags are 0
        // I don't support non-0 flags.
        if (flags)  return -ETODO;

        ssize_t read_len;
        while ((read_len = socket_read(ofd, buf, len)) == -1) {
                block_thread(&sock->blocked_threads);
        }
        return read_len;
}

sysret sys_sendto(int sockfd, const void *buf, size_t len, int flags,
                  const struct sockaddr *addr_g, size_t addrlen) {
        // COPYPASTA from socket_write - collapse these somehow!!!
        // COPYPASTA from socket_write - collapse these somehow!!!
        // COPYPASTA from socket_write - collapse these somehow!!!
        struct open_file *ofd = dmgr_get(&running_process->fds, sockfd);
        if (!ofd)  return -EBADF;

        SOCKET_CHECK_BOILER

        struct sockaddr_in *addr = (struct sockaddr_in *)addr_g;

        // CHEATS
        // BAD
        // FIX
        // TODO: ARP system
        struct mac_addr gw_mac = {{0x52, 0x55, 0x0a, 0x00, 0x02, 0x02}};
        struct mac_addr zero_mac = {{0, 0, 0, 0, 0, 0}};

        uint32_t remote_ip = addr->sin_addr.s_addr;

        size_t ix = 0;
        uint8_t *packet = zmalloc(ETH_MTU);

        ix = make_eth_hdr(packet, gw_mac, zero_mac, ETH_IP);
        struct ip_hdr *ip = (struct ip_hdr *)((char *)packet + ix);
        ix += make_ip_hdr(packet + ix, 0x4050, IPPROTO_UDP, remote_ip, se->my_ip);
        struct udp_pkt *udp = (struct udp_pkt *)((char *)packet + ix);
        ix += make_udp_hdr(packet + ix, se->my_port, addr->sin_port);

        memcpy(packet + ix, buf, len);

        ix += len;
        ip->total_len = htons(ix - sizeof(struct eth_hdr));
        udp->len = htons(len + sizeof(struct udp_pkt));
        place_ip_checksum((struct ip_hdr *)(packet + sizeof(struct eth_hdr)));

        struct net_if *intf = do_routing(remote_ip);
        send_packet(intf, packet, ix);

        free(packet);

        return len; // check for MTU later
}


sysret sys_recvfrom(int sockfd, void *buf, size_t len, int flags,
                    struct sockaddr *addr_g, size_t *addrlen) {
        // COPYPASTA from socket_read - collapse these somehow!!!
        // COPYPASTA from socket_read - collapse these somehow!!!
        // COPYPASTA from socket_read - collapse these somehow!!!
        struct open_file *ofd = dmgr_get(&running_process->fds, sockfd);
        if (!ofd)  return -EBADF;

        SOCKET_CHECK_BOILER

        struct sockaddr_in *addr = (struct sockaddr_in *)addr_g;

        struct datagram *dg;
        while ((dg = list_pop_front(&se->datagrams)) == NULL) {
                block_thread(&node->blocked_threads);
        }

        struct ip_hdr *ip = (struct ip_hdr *)dg->data;
        struct udp_pkt *udp = (struct udp_pkt *)&ip->data;

        size_t data_len = ntohs(udp->len) - 8;

        size_t count = min(data_len, len);
        memcpy(buf, udp->data, count);

        if (addr) {
                addr->sin_family = AF_INET;
                addr->sin_port = ntohs(udp->source_port);
                addr->sin_addr.s_addr = ntohl(ip->source_ip);
        }

        free(dg);

        return count;
}

