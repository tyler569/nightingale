#include <basic.h>
#include <assert.h>
#include <list.h>
#include <ng/fs.h>
#include <ng/thread.h>
#include <sys/socket.h>
#include <sys/un.h>

struct sock_pkt {
    list_node node;
    struct sockaddr_un remote;
    size_t packet_len;
    char packet[];
};

struct socket_file {
    struct file file;
    enum socket_mode mode;
    enum socket_domain domain;
    enum socket_type type;
    enum socket_protocol protocol;

    struct list recv_q;  // datagram
    struct ringbuf ring; // stream

    // read_wq is the main file wq
    struct wq write_wq;

    struct sockaddr_un address;
};

struct file *find_by_sockaddr(struct sockaddr_un *addr, socklen_t len) {
    if (len == sizeof(sa_family_t)) { return NULL; }
    char *path = addr->sun_path;
    return fs_resolve_relative_path(running_thread->cwd, path);
}

struct file *bind_to_path(struct socket_file *sock, struct sockaddr_un *path) {
    struct file *dir =
        fs_resolve_directory_of(running_thread->cwd, path->sun_path);
    sock->address = *path;
    add_dir_file(dir, &sock->file, path->sun_path);
    return &sock->file;
}

ssize_t socket_recvfrom(struct open_file *ofd, void *buffer, size_t len,
                        int flags, struct sockaddr *remote,
                        socklen_t *remote_len) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_SOCKET);
    struct socket_file *sock = (struct socket_file *)file;
    if (sock->type != SOCK_DGRAM) { return -EOPNOTSUPP; }

    while (list_empty(&sock->recv_q)) { wq_block_on(&file->wq); }

    struct sock_pkt *dg = list_pop_front(struct sock_pkt, node, &sock->recv_q);
    if (remote) memcpy(remote, &dg->remote, sizeof(struct sockaddr_un));
    if (remote_len) *remote_len = sizeof(struct sockaddr_un);
    size_t min_len = umin(len, dg->packet_len);
    memcpy(buffer, dg->packet, min_len);
    free(dg);
    return min_len;
}

ssize_t socket_sendto(struct open_file *ofd, const void *buffer, size_t len,
                      int flags, const struct sockaddr *dest,
                      socklen_t dest_len) {
    struct file *file = find_by_sockaddr((struct sockaddr_un *)dest, dest_len);
    if (file->filetype != FT_SOCKET) { return -ECONNREFUSED; }
    struct socket_file *sock = (struct socket_file *)file;
    struct sock_pkt *dg = zmalloc(sizeof(struct sock_pkt) + len);
    dg->remote = sock->address;
    dg->packet_len = len;
    memcpy(dg->packet, buffer, len);
    list_append(&sock->recv_q, &dg->node);
    return len;
}

ssize_t socket_recv(struct open_file *ofd, void *buffer, size_t len,
                    int flags) {
    return socket_recvfrom(ofd, buffer, len, flags, NULL, NULL);
}

ssize_t socket_send(struct open_file *ofd, const void *buffer, size_t len,
                    int flags) {
    return -EAFNOSUPPORT;
}

void socket_close(struct open_file *ofd) {
    ofd->node->signal_eof = 1;
    wq_notify_all(&ofd->node->wq);
}

struct file_ops socket_ops = {
    .close = socket_close,
};

struct socket_ops {
    ssize_t (*recv)(struct open_file *sock, void *buffer, size_t len,
                    int flags);
    ssize_t (*send)(struct open_file *sock, const void *buffer, size_t len,
                    int flags);
    ssize_t (*recvfrom)(struct open_file *sock, void *buffer, size_t len,
                        int flags, struct sockaddr *remote,
                        socklen_t *remote_len);
    ssize_t (*sendto)(struct open_file *sock, const void *buffer, size_t len,
                      int flags, const struct sockaddr *dest,
                      socklen_t dest_len);
};

struct socket_ops socket_dg_sockops = {
    .recv = socket_recv,
    .recvfrom = socket_recvfrom,
    .sendto = socket_sendto,
};



sysret sys_socket(int domain, int type, int protocol) {
    return -ETODO;
}
sysret sys_bind(int sockfd, struct sockaddr const *addr, socklen_t addrlen) {
    return -ETODO;
}
sysret sys_connect(int sock, struct sockaddr const *addr, socklen_t addrlen) {
    return -ETODO;
}
sysret sys_send(int sock, void const *buf, size_t len, int flags) {
    return -ETODO;
}
sysret sys_sendto(int sock, void const *buf, size_t len, int flags,
               struct sockaddr const *remote, socklen_t addrlen) {
    return -ETODO;
}
sysret sys_recv(int sock, void *buf, size_t len, int flags) {
    return -ETODO;
}
sysret sys_recvfrom(int sock, void *buf, size_t len, int flags,
                 struct sockaddr *remote, socklen_t *addrlen) {
    return -ETODO;
}
