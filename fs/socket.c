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
    struct socket_ops *socket_ops;
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
    if (len == sizeof(sa_family_t)) return NULL;
    printf("find_by_sockaddr: '%s'\n", addr->sun_path);
    char *path = addr->sun_path;
    return fs_resolve_relative_path(running_thread->cwd, path);
}

struct file *bind_to_path(struct socket_file *sock, struct sockaddr_un *path) {
    struct file *dir =
        fs_resolve_directory_of(running_thread->cwd, path->sun_path);
    sock->address = *path;
    add_dir_file(dir, &sock->file, strdup(basename(path->sun_path)));
    return &sock->file;
}

ssize_t dg_socket_recvfrom(struct open_file *ofd, void *buffer, size_t len,
                        int flags, struct sockaddr *remote,
                        socklen_t *remote_len) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_SOCKET);
    struct socket_file *sock = (struct socket_file *)file;
    if (sock->type != SOCK_DGRAM) return -EOPNOTSUPP;

    // todo interruptable?
    while (list_empty(&sock->recv_q)) { wq_block_on(&file->wq); }

    struct sock_pkt *dg = list_pop_front(struct sock_pkt, node, &sock->recv_q);
    if (remote) memcpy(remote, &dg->remote, sizeof(struct sockaddr_un));
    if (remote_len) *remote_len = sizeof(struct sockaddr_un);
    size_t min_len = umin(len, dg->packet_len);
    memcpy(buffer, dg->packet, min_len);
    free(dg);
    return min_len;
}

ssize_t dg_socket_sendto(struct open_file *ofd, const void *buffer, size_t len,
                      int flags, const struct sockaddr *dest,
                      socklen_t dest_len) {
    struct file *file = find_by_sockaddr((struct sockaddr_un *)dest, dest_len);
    if (!file) return -EACCES;
    if (file->filetype != FT_SOCKET) return -ECONNREFUSED;
    // check that it's a datagram socket
    struct socket_file *sock = (struct socket_file *)file;
    struct sock_pkt *dg = zmalloc(sizeof(struct sock_pkt) + len);
    dg->remote = sock->address;
    dg->packet_len = len;
    memcpy(dg->packet, buffer, len);
    list_append(&sock->recv_q, &dg->node);
    return len;
}

ssize_t dg_socket_recv(struct open_file *ofd, void *buffer, size_t len,
                    int flags) {
    return dg_socket_recvfrom(ofd, buffer, len, flags, NULL, NULL);
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
    .recv = dg_socket_recv,
    .recvfrom = dg_socket_recvfrom,
    .sendto = dg_socket_sendto,
};


#define GET(fd) \
    struct open_file *ofd = dmgr_get(&running_process->fds, sock); \
    if (!ofd) return -EBADF; \
    struct file *file = ofd->node; \
    if (file->filetype != FT_SOCKET) return -ENOTSOCK; \
    struct socket_file *socket = (struct socket_file *)file;


sysret sys_socket(int domain, int type, int protocol) {
    if (domain != AF_UNIX) return -EAFNOSUPPORT;
    if (type != SOCK_DGRAM) return -ETODO;
    if (protocol != PROTO_DEFAULT) return -EPROTONOSUPPORT;

    struct socket_file *socket = zmalloc(sizeof(struct socket_file));
    socket->file.filetype = FT_SOCKET;
    socket->file.ops = &socket_ops;
    socket->file.permissions = USR_READ | USR_WRITE;
    socket->mode = SOCKET_IDLE;
    socket->domain = domain;
    socket->type = type;
    socket->protocol = protocol;

    wq_init(&socket->file.wq);
    wq_init(&socket->write_wq);

    list_init(&socket->recv_q);
    // _or_ ring_emplace for stream
    socket->socket_ops = &socket_dg_sockops;

    return do_open(&socket->file, NULL, USR_READ | USR_WRITE, 0);
}

sysret sys_bind(int sock, struct sockaddr const *addr, socklen_t addrlen) {
    GET(sock);
    bind_to_path(socket, (struct sockaddr_un *)addr);
    return 0;
}

sysret sys_connect(int sock, struct sockaddr const *addr, socklen_t addrlen) {
    GET(sock);
    return -EOPNOTSUPP;
}

sysret sys_send(int sock, void const *buf, size_t len, int flags) {
    GET(sock);
    sysret r = -EOPNOTSUPP;
    if (socket->socket_ops->send) {
        r = socket->socket_ops->send(ofd, buf, len, flags);
    }
    return r;
}

sysret sys_sendto(int sock, void const *buf, size_t len, int flags,
               struct sockaddr const *remote, socklen_t addrlen) {
    GET(sock);
    sysret r = -EOPNOTSUPP;
    if (socket->socket_ops->sendto) {
        r = socket->socket_ops->sendto(ofd, buf, len, flags, remote, addrlen);
    }
    return r;
}

sysret sys_recv(int sock, void *buf, size_t len, int flags) {
    GET(sock);
    sysret r = -EOPNOTSUPP;
    if (socket->socket_ops->recv) {
        r = socket->socket_ops->recv(ofd, buf, len, flags);
    }
    return r;
}

sysret sys_recvfrom(int sock, void *buf, size_t len, int flags,
                 struct sockaddr *remote, socklen_t *addrlen) {
    GET(sock);
    sysret r = -EOPNOTSUPP;
    if (socket->socket_ops->recvfrom) {
        r = socket->socket_ops->recvfrom(ofd, buf, len, flags, remote, addrlen);
    }
    return r;
}
