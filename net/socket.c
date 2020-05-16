
#include <basic.h>
#include <ng/mutex.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <errno.h>
#include <net/net.h>
#include <net/socket.h>

void tcp_send(struct socket_impl *, const void *, size_t);
void tcp_status(struct socket_impl *, enum tcp_flags);

#define N_MAXSOCKETS 256

struct socket_impl sockets[N_MAXSOCKETS] = {0};

static int next_avail() {
        int i = -1;
        for (i=0; i<N_MAXSOCKETS; i++) {
                if (!sockets[i].valid) {
                        sockets[i].valid = true;
                        break;
                }
        }
        return i;
}

int i_socket(int domain, int type, int protocol) {
        int i = next_avail();
        if (i == -1) {
                errno = ENOMEM;
                return -1;
        }

        struct socket_impl *s = sockets + i;

        if (domain != AF_INET) {
                errno = EAFNOSUPPORT;
                return -1;
        }

        // TODO we could validate these inputs
        if (type == SOCK_STREAM) protocol = IPPROTO_TCP;
        if (type == SOCK_DGRAM) protocol = IPPROTO_UDP;

        s->domain = domain;     // AF_INET
        s->type = type;         // SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW
        s->protocol = protocol; // IPPROTO_TCP, IPPROTO_UDP, or IP protocol #
        s->state = SOCKET_REQUESTED;

        if (type == SOCK_DGRAM) {
                list_init(&s->dgram_queue);
        } else if (type == SOCK_STREAM) {
                list_init(&s->ooo_queue);
                list_init(&s->unacked_pks);
        }

        if (protocol == IPPROTO_TCP) {
                s->recv_buf = malloc(TCP_RECV_BUF_LEN);
        }

        KMUTEX_INIT_LIVE(s->block_mtx);

        return i;
}

struct socket_impl *resolve_sockfd(int sockfd) {
        struct socket_impl *s = sockets + sockfd;
        if (s->valid) {
                return s;
        } else {
                errno = ENOTSOCK;
                return NULL;
        }
}

int x_bind(struct socket_impl *s, const struct sockaddr *addr, socklen_t addrlen) {
        if (addrlen != sizeof(struct sockaddr_in)) {
                errno = EAFNOSUPPORT;
                return -1;
        }

        struct sockaddr_in *in_addr = (struct sockaddr_in *)addr;
        if (in_addr->sin_family != AF_INET) {
                errno = EAFNOSUPPORT;
                return -1;
        }

        s->local_ip = in_addr->sin_addr.s_addr;
        s->local_port = in_addr->sin_port;

        s->state = SOCKET_BOUND;
        return 0;
}

int x_listen(struct socket_impl *s, int backlog) {
        list_init(&s->accept_queue);
        s->state = SOCKET_LISTENING;
        s->tcp_state = TCP_S_LISTEN;
        return 0;
}

int x_accept(struct socket_impl *s, struct sockaddr *addr, socklen_t *addrlen) {
        if (!(s->type == SOCK_STREAM)) {
                errno = EOPNOTSUPP;
                return -1;
        }

        struct pkb *accept_pk;
        do {
                mutex_await(&s->block_mtx);
        } while(!list_head(&s->accept_queue));

        accept_pk = list_pop_front(struct pkb, &s->accept_queue, queue);
        struct ip_header *accept_ip = ip_hdr(accept_pk);
        struct tcp_header *accept_tcp = tcp_hdr(accept_ip);
        assert(accept_tcp->f_syn && !accept_tcp->f_ack);

        int i = next_avail();
        if (i == -1) {
                errno = ENOMEM;
                return -1;
        }

        struct socket_impl *as = sockets + i;

        memcpy(as, s, sizeof(struct socket_impl));
        KMUTEX_INIT_LIVE(as->block_mtx);

        // list_init(&as->accept_queue);
        list_init(&as->ooo_queue);
        list_init(&as->unacked_pks);

        as->state = SOCKET_OUTBOUND;
        as->tcp_state = TCP_S_SYN_RECIEVED;
        as->remote_ip = accept_ip->source_ip;
        as->remote_port = accept_tcp->source_port;
        as->recv_seq = ntohl(accept_tcp->seq);

        if (as->local_ip == 0) {
                as->local_ip = accept_pk->from->ip;
        }

        struct sockaddr_in in_addr = {
                .sin_family = AF_INET,
                .sin_port = as->remote_port,
                .sin_addr = {as->remote_ip},
        };

        memcpy(addr, &in_addr, sizeof(in_addr));
        *addrlen = sizeof(in_addr);

        as->recv_seq += 1;
        tcp_status(as, TCP_SYN | TCP_ACK);

        return i;
}

int tcp_connect(struct socket_impl *s, const struct sockaddr_in *addr) {
        s->remote_ip = addr->sin_addr.s_addr;
        s->remote_port = addr->sin_port;
        s->state = SOCKET_OUTBOUND;

        s->send_seq = rand();
        s->send_ack = 0;
        s->recv_seq = 0;

        tcp_status(s, TCP_SYN);
        s->tcp_state = TCP_S_SYN_SENT;

        do {
                mutex_await(&s->block_mtx);
        } while (s->tcp_state == TCP_S_SYN_SENT);

        if (s->tcp_state == TCP_S_ESTABLISHED) {
                return 0;
        } else if (s->tcp_state == TCP_S_CLOSED) {
                errno = ECONNREFUSED;
                return -1;
        } else {
                printf("I'm not sure how we got to %i\n", s->tcp_state);
                errno = 1005;
                return -1;
        }
}

int x_connect(struct socket_impl *s, const struct sockaddr *addr, socklen_t addrlen) {
        const struct sockaddr_in *in_addr = (const struct sockaddr_in *)addr;
        if (in_addr->sin_family != AF_INET) {
                errno = EAFNOSUPPORT;
                return -1;
        }

        if (s->protocol != IPPROTO_TCP) {
                errno = EPROTO;
                return -1;
        }

        return tcp_connect(s, in_addr);
}

void udp_send(struct socket_impl *s, const void *data, size_t len) {
        if (s->remote_ip == 0) {
                // they should use sendto
                return; // indicate error?
        }

        struct pkb *pk = new_pk();
        make_udp(s, pk, NULL, data, len);
        dispatch(pk);
        free_pk(pk);

        s->ip_id += 1;
}

ssize_t x_send(struct socket_impl *s, const void *buf, size_t len, int flags) {
        if (s->state != SOCKET_OUTBOUND) {
                errno = EDESTADDRREQ;
                return -1;
        }

        if (s->type == SOCK_DGRAM) {
                udp_send(s, buf, len);
        }

        if (s->type == SOCK_STREAM) {
                tcp_send(s, buf, len);
        }
        return len;
}

void udp_sendto(struct socket_impl *s, struct sockaddr_in *d_addr,
                const void *data, size_t len) {
        struct pkb *pk = new_pk();
        make_udp(s, pk, d_addr, data, len);
        dispatch(pk);
        free_pk(pk);

        s->ip_id += 1;
}

ssize_t x_sendto(struct socket_impl *s, const void *buf, size_t len, int flags,
                const struct sockaddr *dest_addr, socklen_t addrlen) {
        if (s->type != SOCK_DGRAM) {
                errno = EFAULT; // TODO
                return -1;
        }

        struct sockaddr_in *in_addr = (struct sockaddr_in *)dest_addr;
        if (addrlen != sizeof(*in_addr)) {
                errno = EFAULT; // TODO ? How do you handle this?
                return -1;
        }

        udp_sendto(s, in_addr, buf, len);
        return len; // what if we sent less?
}

ssize_t x_recv(struct socket_impl *s, void *buf, size_t len, int flags) {
        printf("x_RECV\n");
        size_t buf_ix = 0;

        while (buf_ix < len) {
                mutex_await(&s->block_mtx);

                if (s->tcp_state != TCP_S_ESTABLISHED) {
                        // done
                        return 0;
                }

                size_t available = min(len - buf_ix, s->recv_buf_len);

                if (available > 0) {
                        memcpy(buf + buf_ix, s->recv_buf, available);
                        buf_ix += available;

                        memmove(s->recv_buf, s->recv_buf + available, available);
                        s->recv_buf_seq += available;
                        s->recv_buf_len -= available;
                }

                if (s->tcp_psh) {
                        s->tcp_psh = false;
                        break;
                }
        }

        return buf_ix;
}

ssize_t x_recvfrom(struct socket_impl *s, void *buf, size_t len, int flags,
                struct sockaddr *src_addr, socklen_t *addrlen) {
        struct pkb *recv_pk;
        do {
                mutex_await(&s->block_mtx);
        } while (!list_head(&s->dgram_queue));

        recv_pk = list_pop_front(struct pkb, &s->dgram_queue, queue);
        struct ip_header *recv_ip = ip_hdr(recv_pk);
        struct udp_header *recv_udp = udp_hdr(recv_ip);
        int udp_data_len = udp_len(recv_pk);
        void *udp_d = udp_data(recv_pk);

        struct sockaddr_in *in_addr = (struct sockaddr_in *)src_addr;
        if (*addrlen < sizeof(*in_addr)) {
                errno = EFAULT;
                return -1;
        }
        in_addr->sin_family = AF_INET;
        in_addr->sin_port = recv_udp->source_port;
        in_addr->sin_addr.s_addr = recv_ip->source_ip;
        *addrlen = sizeof(*in_addr);

        len = (udp_data_len < len) ? udp_data_len : len;
        memcpy(buf, udp_d, len);

        return len;
}

int x_close(struct socket_impl *s) {
        // teardown connections ?

        s->state = SOCKET_IDLE;
        s->valid = false;
        return 0;
}

int i_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
        struct socket_impl *s = resolve_sockfd(sockfd);
        if (s) {
                return x_bind(s, addr, addrlen);
        } else {
                return -1;
        }
}

int i_listen(int sockfd, int backlog) {
        struct socket_impl *s = resolve_sockfd(sockfd);
        if (s) {
                return x_listen(s, backlog);
        } else {
                return -1;
        }
}

int i_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
        struct socket_impl *s = resolve_sockfd(sockfd);
        if (s) {
                return x_accept(s, addr, addrlen);
        } else {
                return -1;
        }
}

int i_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
        struct socket_impl *s = resolve_sockfd(sockfd);
        if (s) {
                return x_connect(s, addr, addrlen);
        } else {
                return -1;
        }
}

ssize_t i_send(int sockfd, const void *buf, size_t len, int flags) {
        struct socket_impl *s = resolve_sockfd(sockfd);
        if (s) {
                return x_send(s, buf, len, flags);
        } else {
                return -1;
        }
}

ssize_t i_sendto(int sockfd, const void *buf, size_t len, int flags,
                const struct sockaddr *dest_addr, socklen_t addrlen) {
        struct socket_impl *s = resolve_sockfd(sockfd);
        if (s) {
                return x_sendto(s, buf, len, flags, dest_addr, addrlen);
        } else {
                return -1;
        }
}

ssize_t i_recv(int sockfd, void *buf, size_t len, int flags) {
        struct socket_impl *s = resolve_sockfd(sockfd);
        if (s) {
                return x_recv(s, buf, len, flags);
        } else {
                return -1;
        }
}

ssize_t i_recvfrom(int sockfd, void *buf, size_t len, int flags,
                struct sockaddr *src_addr, socklen_t *addrlen) {
        struct socket_impl *s = resolve_sockfd(sockfd);
        if (s) {
                return x_recvfrom(s, buf, len, flags, src_addr, addrlen);
        } else {
                return -1;
        }
}

int i_close(int sockfd) {
        struct socket_impl *s = resolve_sockfd(sockfd);
        if (s) {
                return x_close(s);
        } else {
                return -1;
        }
}

//
// DISPATCH
//

void socket_dispatch_udp(struct pkb *pk) {
        printf("dispatching udp\n");
        struct ip_header *ip = ip_hdr(pk);
        struct udp_header *udp = udp_hdr(ip);

        int best_match = -1;
        for (int i=0; i<N_MAXSOCKETS; i++) {
                struct socket_impl *s = sockets + i;
                bool outbound = s->state == SOCKET_OUTBOUND;
                bool listening = s->state == SOCKET_LISTENING;

                if (!s->valid) continue;
                if (s->domain != AF_INET) continue;
                if (s->type != SOCK_STREAM) continue;
                if (s->protocol != IPPROTO_UDP) continue;

                if (outbound) {
                        if (s->local_ip != ip->destination_ip) continue;
                } else if (listening) {
                        if (s->local_ip != ip->destination_ip && s->local_ip != 0) continue;
                }

                if (s->local_port != udp->destination_port) continue;

                if (outbound) {
                        if (s->remote_ip != ip->source_ip) continue;
                        if (s->remote_port != udp->source_port) continue;
                }

                best_match = i;
        }
        if (best_match == -1) {
                // no matching socket, drop.
                printf("udp: no matching socket\n");
                return;
        }
        printf("udp dispatch found match: %i\n", best_match);
        struct socket_impl *s = sockets + best_match;

        pk_incref(pk);
        list_append(&s->dgram_queue, pk, queue);

        mutex_signal(&s->block_mtx, 1);
}

void tcp_status(struct socket_impl *s, enum tcp_flags flags) {
        struct pkb *pk = new_pk();
        make_tcp(s, pk, flags, NULL, 0);
        dispatch(pk);
        free_pk(pk);

        s->ip_id += 1;

        if (flags & TCP_SYN) {
                s->send_seq += 1;
        }

        if (flags & TCP_FIN) {
                s->send_seq += 1;
        }
}

void tcp_send(struct socket_impl *s, const void *data, size_t len) {
        struct pkb *pk = new_pk();
        make_tcp(s, pk, TCP_PSH | TCP_ACK, data, len);
        dispatch(pk);
        free_pk(pk);

        s->ip_id += 1;
        s->send_seq += len;
}

void require_that(bool x) {}

void socket_dispatch_tcp(struct pkb *pk) {
        struct ip_header *ip = ip_hdr(pk);
        struct tcp_header *tcp = tcp_hdr(ip);

        int best_match = -1;
        for (int i=0; i<N_MAXSOCKETS; i++) {
                struct socket_impl *s = sockets + i;
                bool outbound = s->state == SOCKET_OUTBOUND;
                bool listening = s->state == SOCKET_LISTENING;

                if (!s->valid) continue;
                if (s->domain != AF_INET) continue;
                if (s->type != SOCK_STREAM) continue;
                if (s->protocol != IPPROTO_TCP) continue;

                if (outbound) {
                        if (s->local_ip != ip->destination_ip) continue;
                } else if (listening) {
                        if (s->local_ip != ip->destination_ip && s->local_ip != 0) continue;
                }

                if (s->local_port != tcp->destination_port) continue;

                if (outbound) {
                        if (s->remote_ip != ip->source_ip) continue;
                        if (s->remote_port != tcp->source_port) continue;
                }

                best_match = i;
        }
        if (best_match == -1) {
                // no matching socket, drop.
                printf("tcp: no matching socket\n");
                return;
        }
        printf("tcp dispatch found match: %i\n", best_match);
        struct socket_impl *s = sockets + best_match;

        uint32_t tcp_rseq = ntohl(tcp->seq);
        uint32_t tcp_rack = ntohl(tcp->ack);

        if (s->tcp_state == TCP_S_SYN_RECIEVED && tcp->f_ack) {
                if (tcp_rack == s->send_seq) {
                        s->tcp_state = TCP_S_ESTABLISHED;
                        return;
                }
        }

        // ACK sequence update
        if (tcp->f_ack) {
                s->send_ack = tcp_rack;

                // TODO: drop retx queue that have been fully ack'd
        }

        // RST -> close
        if (s->tcp_state == TCP_S_SYN_SENT && tcp->f_rst) {
                s->tcp_state = TCP_S_CLOSED;

                mutex_signal(&s->block_mtx, 1);
        }

        if (s->tcp_state == TCP_S_LISTEN && tcp->f_syn) {
                pk_incref(pk);
                list_append(&s->accept_queue, pk, queue);

                mutex_signal(&s->block_mtx, 1);

                // x_accept creates a new socket and runs the SYN_ACK from there
        }

        // SYN/ACK -> Established
        if (s->tcp_state == TCP_S_SYN_SENT && tcp->f_syn && tcp->f_ack) {
                require_that(s->send_ack == s->send_seq);
                s->recv_seq = tcp_rseq + 1; // SYN is ~ 1 byte
                tcp_status(s, TCP_ACK);

                s->tcp_state = TCP_S_ESTABLISHED;

                mutex_signal(&s->block_mtx, 1);
        }

        // FIN -> FIN/ACK
        if (s->tcp_state == TCP_S_ESTABLISHED && tcp->f_fin) {
                s->recv_seq = tcp_rseq + 1; // FIN counts as a byte
                tcp_status(s, TCP_FIN | TCP_ACK);

                s->tcp_state = TCP_S_CLOSE_WAIT;

                mutex_signal(&s->block_mtx, 1);
        }

        if (s->tcp_state == TCP_S_CLOSING && tcp->f_ack) {
                s->tcp_state = TCP_S_CLOSED;
                // is this is the ack _for the FIN_ ?

                mutex_signal(&s->block_mtx, 1);
        }

        uint32_t start_rseq = tcp_rseq;
        uint32_t end_rseq = tcp_rseq + tcp_len(pk);

        if (tcp->f_syn || tcp->f_fin) {
                end_rseq += 1;
        }

        if (s->tcp_state == TCP_S_ESTABLISHED) {
                // TODO ^^ MODULO 2**32

                if (s->recv_seq == start_rseq) {
                        int tcp_length = tcp_len(pk);
                        if (tcp_length + s->recv_buf_len > TCP_RECV_BUF_LEN) {
                                // too slow!
                                // TODO: tcp_ooo_insert(ooo_queue)
                                //       s->pending_slow_data = true
                        } else {
                                void *tcp_d = tcp_data(pk);
                                memcpy(s->recv_buf + s->recv_buf_len, tcp_d, tcp_length);
                                s->recv_buf_len += tcp_length;

                                s->recv_seq = end_rseq;
                                tcp_status(s, TCP_ACK);

                                /*
                                   if (list_head(&s->ooo_queue)) {
                                // TODO: try to apply ooo data pending
                                }
                                */

                                // TODO: check if needs signal?
                                // not sure how this works tbh

                                if (tcp->f_psh) {
                                        // signal userspace should process ASAP
                                        s->tcp_psh = true;
                                }
                                mutex_signal(&s->block_mtx, 1);
                        }
                }
        }

        // RST
        if (tcp->f_rst) {
                printf("TCP RST\n");
                s->tcp_state = TCP_S_CLOSED;
                return;
        }
}

