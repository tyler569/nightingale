
#include <basic.h>
#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <panic.h>
#include <fs/vfs.h>
#include <thread.h>
#include <net/net_if.h>
#include <net/ether.h>
#include <net/ip.h>
#include <net/udp.h>
#include <syscall.h>
#include <syscalls.h>
#include "socket.h"

struct vector socket_table = {0};

uint64_t flow_hash(
        uint32_t myip, uint32_t othrip, uint16_t myport, uint16_t othrport) {
    uint64_t r = 103;
    r *= myip * 7;
    r *= othrip * 13;
    r *= myport * 5;
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

    size_t fs_node_handle;

    struct net_if *intf;

    uint16_t my_port;
    uint16_t othr_port;
    uint32_t my_ip;
    uint32_t othr_ip;
    uint64_t flow_hash;

    union {
        struct queue dg;
        struct ringbuf st;
    };
};

void socket_dispatch(struct ip_hdr* ip) {
    struct udp_pkt* udp = (struct udp_pkt*)ip->data;
    uint64_t hash = flow_hash(
            ntohl(ip->dst_ip), ntohl(ip->src_ip),
            ntohs(udp->dst_port), ntohs(udp->src_port));

    struct socket_extra* extra = 0;

    for (size_t i=0; i<socket_table.len; i++) {
        extra = vec_get(&socket_table, i);
        // printf("hashes: %#lx %#lx\n", extra->flow_hash, hash);
        if (extra->flow_hash == hash) {
            // printf("found associated flow!\n");
            break;
        }
    }

    if (extra == 0) {
        return;
    }

    size_t len = ntohs(udp->len) - 8;

    // printf("dg.last -> %#lx\n", extra->dg.last);
    
    struct queue_object* qo = malloc(
            sizeof(struct queue_object) + sizeof(struct datagram) + len);
    
    struct datagram* dg = (struct datagram*)&qo->data;
    dg->len = len;
    memcpy(&dg->data, &udp->data, len);

    queue_enqueue(&extra->dg, qo);

    struct fs_node* file = vec_get(fs_node_table, extra->fs_node_handle);
    wake_blocked_threads(&file->blocked_threads);
}

ssize_t socket_read(struct fs_node* sock_node, void* data, size_t len) {
    assert(sock_node->filetype = NET_SOCK, "only sockets should get here");
    struct socket_extra* sock = vec_get(&socket_table, sock_node->extra_handle);

    if (sock->af_type != AF_INET) {
        panic("Unsupported AF_TYPE: %i\n", sock->af_type);
    }
    if (sock->sock_type != SOCK_DGRAM) {
        panic("Unsupported SOCK_TYPE: %i\n", sock->sock_type);
    }
    if (sock->protocol != SOCK_DEFAULT && sock->protocol != PROTO_UDP) {
        panic("Unsupported protocol: %i\n", sock->protocol);
    }

    // printf("waiting on %#lx\n", dg);
    struct queue_object* qo;
    if (!(qo = queue_dequeue(&sock->dg))) {
        return -1;
    }
    struct datagram* dg = (struct datagram*)&qo->data;

    size_t count = min(dg->len, len);
    memcpy(data, dg->data, count);

    free(qo);

    return count;
}

ssize_t socket_write(struct fs_node* sock_node, void const* data, size_t len) {
    assert(sock_node->filetype = NET_SOCK, "only sockets should get here");
    struct socket_extra* sock = vec_get(&socket_table, sock_node->extra_handle);

    // CHEATS
    // BAD
    // FIX
    // TODO: ARP system
    struct mac_addr gw_mac = {{ 0x52, 0x55, 0x0a, 0x00, 0x02, 0x02 }};
    struct mac_addr zero_mac = {{ 0, 0, 0, 0, 0, 0 }};

    if (sock->af_type != AF_INET) {
        panic("Unsupported AF_TYPE: %i\n", sock->af_type);
    }
    if (sock->sock_type != SOCK_DGRAM) {
        panic("Unsupported SOCK_TYPE: %i\n", sock->sock_type);
    }
    if (sock->protocol != SOCK_DEFAULT && sock->protocol != PROTO_UDP) {
        panic("Unsupported protocol: %i\n", sock->protocol);
    }

    // struct sock_dgram *dg = (void *)sock; // TEMP

    size_t ix = 0;
    uint8_t* packet = calloc(ETH_MTU, 1);

    ix = make_eth_hdr(packet, gw_mac, zero_mac, ETH_IP);
    struct ip_hdr* ip = (struct ip_hdr*)((char*)packet + ix);
    ix += make_ip_hdr(packet + ix, 0x4050, PROTO_UDP, sock->othr_ip);
    struct udp_pkt* udp = (struct udp_pkt*)((char*)packet + ix);
    ix += make_udp_hdr(packet + ix, sock->my_port, sock->othr_port);

    memcpy(packet + ix, data, len);

    ix += len;
    ip->total_len = htons(ix - sizeof(struct eth_hdr));
    udp->len = htons(len + sizeof(struct udp_pkt));
    place_ip_checksum((struct ip_hdr *)(packet + sizeof(struct eth_hdr)));
    send_packet(sock->intf, packet, ix);
    free(packet);

    return len; // check for MTU later
}

struct syscall_ret sys_socket(int domain, int type, int protocol) {
    struct syscall_ret ret = {0};
    if (domain != AF_INET) {
        ret.error = EAFNOSUPPORT;
        return ret;
    }
    if (type != SOCK_DGRAM) {
        ret.error = EINVAL;
        return ret;
    }
    if (protocol != PROTO_UDP) {
        ret.error = EINVAL;
        return ret;
    }

    struct socket_extra extra = {
        .af_type = domain,
        .sock_type = type,
        .protocol = protocol,
        .intf = NULL, // set at bind() I think
        // same for ports and ips and flow hashes
        .dg = {0},
    };
    uintptr_t extra_handle = vec_push(&socket_table, &extra);
    struct socket_extra* pextra = vec_get(&socket_table, extra_handle);

    struct fs_node new_sock = {
        .filetype = NET_SOCK,
        .permission = USR_READ | USR_WRITE,
        .read = socket_read,
        .write = socket_write,
        .extra_handle = extra_handle,
    };

    size_t new_file_id = vec_push(fs_node_table, &new_sock);
    size_t new_fd = vec_push_value(&running_process->fds, new_file_id);

    pextra->fs_node_handle = new_file_id;
    
    RETURN_VALUE(new_fd);
}

struct syscall_ret sys_bind(int sockfd, struct sockaddr* _addr, socklen_t addrlen) {
    struct syscall_ret ret = {0};
    if (addrlen != 16) {
        ret.error = EINVAL;
        return ret;
    }
    size_t file_number = vec_get_value(&running_process->fds, sockfd);
    struct fs_node* sock = vec_get(fs_node_table, file_number);
    if (sock->filetype != NET_SOCK) {
        ret.error = EINVAL;
        return ret;
    }
    struct socket_extra* extra = vec_get(&socket_table, sock->extra_handle);

    // something something check to make sure the address isn't already in use

    struct sockaddr_in* addr = (void*)_addr;
    extra->my_ip = addr->sin_addr.s_addr;
    extra->my_port = addr->sin_port;
    // extra->intf = nic;  // static, passed to init

    return ret;
}


struct syscall_ret sys_connect(int sockfd, struct sockaddr* _addr, socklen_t addrlen) {
    struct syscall_ret ret = {0};
    if (addrlen != 16) {
        ret.error = EINVAL;
        return ret;
    }
    size_t file_number = vec_get_value(&running_process->fds, sockfd);
    struct fs_node* sock = vec_get(fs_node_table, file_number);
    if (sock->filetype != NET_SOCK) {
        ret.error = EINVAL;
        return ret;
    }
    struct socket_extra* extra = vec_get(&socket_table, sock->extra_handle);

    // something something different behavior for SOCK_STREAM
    // TCP does a lot here

    struct sockaddr_in* addr = (void*)_addr;
    extra->othr_port = addr->sin_port;
    extra->othr_ip = addr->sin_addr.s_addr;
    extra->flow_hash = flow_hash(
        extra->my_ip,
        extra->othr_ip,
        extra->my_port,
        extra->othr_port
    );

    return ret;
}


struct syscall_ret sys_send(int sockfd, const void* buf, size_t len, int flags) {
    struct syscall_ret ret = {0};
    size_t file_number = vec_get_value(&running_process->fds, sockfd);
    struct fs_node* sock_node = vec_get(fs_node_table, file_number);
    assert(sock_node->filetype = NET_SOCK, "only sockets should get here");

    // send is just write if the flags are 0
    // I don't support non-0 flags.
    if (flags) {
        ret.error = EINVAL;
        return ret;
    }
    ret.value = socket_write(sock_node, buf, len);
    return ret;
}

struct syscall_ret sys_sendto(int sockfd, const void* buf, size_t len, int flags,
                              const struct sockaddr* addr, size_t addrlen) {
    struct syscall_ret ret = {0};
    size_t file_number = vec_get_value(&running_process->fds, sockfd);
    struct fs_node* sock_node = vec_get(fs_node_table, file_number);
    assert(sock_node->filetype = NET_SOCK, "only sockets should get here");

    ret.error = -1; // unimplemented
    return ret;
}


struct syscall_ret sys_recv(int sockfd, void* buf, size_t len, int flags) {
    struct syscall_ret ret = {0};
    size_t file_number = vec_get_value(&running_process->fds, sockfd);
    struct fs_node* sock_node = vec_get(fs_node_table, file_number);
    assert(sock_node->filetype = NET_SOCK, "only sockets should get here");

    // recv is just read if the flags are 0
    // I don't support non-0 flags.
    if (flags) {
        ret.error = EINVAL;
        return ret;
    }
    while ((ret.value = socket_read(sock_node, buf, len)) == -1) {
        block_thread(&sock_node->blocked_threads);
    }
    return ret;
}

struct syscall_ret sys_recvfrom(int sockfd, void* buf, size_t len, int flags, 
                                struct sockaddr* addr, size_t* addrlen) {
    struct syscall_ret ret = {0};
    size_t file_number = vec_get_value(&running_process->fds, sockfd);
    struct fs_node* sock_node = vec_get(fs_node_table, file_number);
    assert(sock_node->filetype = NET_SOCK, "only sockets should get here");
    
    ret.error = -1; // unimplemented
    return ret;
}

