#include <basic.h>
#include <assert.h>
#include <list.h>
#include <ng/fs.h>
#include <ng/ringbuf.h>
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

    struct list recv_q;  // datagram OR inbound connections
    struct ringbuf ring; // stream

    // read_wq is the main file wq
    struct wq write_wq;

    struct sockaddr_un address;
    struct socket_file *pair;
};

sysret sys_socket(int, int, int);

struct socket_ops;
struct socket_ops socket_st_sockops;
struct socket_ops socket_dg_sockops;
struct socket_ops socket_lsn_sockops;

static struct file *find_by_sockaddr(struct sockaddr_un *addr, socklen_t len) {
    if (len == sizeof(sa_family_t)) return NULL;
    char *path = addr->sun_path;
    return fs_resolve_relative_path(running_thread->cwd, path);
}

static sysret bind_to_path(struct socket_file *sock, struct sockaddr_un *path) {
    if (sock->mode != SOCKET_IDLE)
        return -EINVAL; // TODO this is probably wrong
    struct file *dir =
        fs_resolve_directory_of(running_thread->cwd, path->sun_path);
    // TODO: EADDRINUSE if the file resolves
    sock->address = *path;
    sock->mode = SOCKET_BOUND;
    return add_dir_file(dir, &sock->file, strdup(basename(path->sun_path)));
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
    struct socket_file *sock = (struct socket_file *)file;
    if (sock->type != SOCK_DGRAM) return -EOPNOTSUPP;
    struct sock_pkt *dg = zmalloc(sizeof(struct sock_pkt) + len);
    dg->remote = sock->address;
    dg->packet_len = len;
    memcpy(dg->packet, buffer, len);
    list_append(&sock->recv_q, &dg->node);
    wq_notify_all(&file->wq);
    return len;
}

ssize_t dg_socket_recv(struct open_file *ofd, void *buffer, size_t len,
                       int flags) {
    return dg_socket_recvfrom(ofd, buffer, len, flags, NULL, NULL);
}

void socket_close(struct open_file *n) {
    struct file *file = n->node;
    if (--file->refcnt > 1) {
        return;
    }

    struct socket_file *socket = (struct socket_file *)file;
    if (socket->pair) {
        assert(socket->pair->pair == socket);
        socket->pair->file.signal_eof = 1;
        wq_notify_all(&socket->pair->file.wq);
        socket->pair->pair = NULL;
    }
    // free anything in the queue
    wq_notify_all(&socket->write_wq);
    // ring_free(&socket->ring); // IFF the ring is allocated
    free(socket);
}

struct inbound_connection {
    list_node node;
    struct socket_file *socket;
};

int st_socket_listen(struct open_file *ofd, int backlog) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_SOCKET);
    struct socket_file *socket = (struct socket_file *)file;
    assert(socket->domain == AF_UNIX);
    assert(socket->type == SOCK_STREAM);
    if (socket->mode != SOCKET_BOUND) return -ENOTCONN;

    // TODO support backlog
    list_init(&socket->recv_q);
    socket->mode = STREAM_LISTENING;

    return 0;
}

int st_socket_connect(struct open_file *ofd, const struct sockaddr *addr,
                      socklen_t len) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_SOCKET);
    struct socket_file *socket = (struct socket_file *)file;
    assert(socket->domain == AF_UNIX);
    assert(socket->type == SOCK_STREAM);

    struct file *cfile = find_by_sockaddr((struct sockaddr_un *)addr, len);
    if (!cfile) return -EACCES;
    if (cfile->filetype != FT_SOCKET) return -ECONNREFUSED;
    struct socket_file *csocket = (struct socket_file *)cfile;
    if (csocket->type != SOCK_STREAM) return -EOPNOTSUPP;

    struct inbound_connection *c = malloc(sizeof(struct inbound_connection));
    c->socket = socket;
    list_append(&csocket->recv_q, &c->node);
    wq_notify_all(&csocket->file.wq);
    wq_block_on(&file->wq); // accept(2) wakes us back up

    return 0;
}

int st_socket_accept(struct open_file *ofd, struct sockaddr *addr,
                     socklen_t *len) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_SOCKET);
    struct socket_file *socket = (struct socket_file *)file;
    if (socket->type != SOCK_STREAM) return -EOPNOTSUPP;

    while (list_empty(&socket->recv_q)) { wq_block_on(&file->wq); }

    struct inbound_connection *c =
        list_pop_front(struct inbound_connection, node, &socket->recv_q);

    int fd = sys_socket(AF_UNIX, SOCK_STREAM, 0);
    struct open_file *myfd = dmgr_get(&running_process->fds, fd);
    struct file *myfile = myfd->node;
    struct socket_file *mysocket = (struct socket_file *)myfile;

    mysocket->pair = c->socket;
    c->socket->pair = mysocket;
    mysocket->mode = STREAM_CONNECTED;
    c->socket->mode = STREAM_CONNECTED;

    mysocket->socket_ops = &socket_st_sockops;
    c->socket->socket_ops = &socket_st_sockops;

    memcpy(addr, &c->socket->address, *len);
    *len = min(sizeof(struct sockaddr_un), *len);

    wq_notify_all(&c->socket->file.wq); // wake the connector back up

    return fd;
}

ssize_t st_socket_recv(struct open_file *ofd, void *buffer, size_t len,
                       int flags) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_SOCKET);
    struct socket_file *socket = (struct socket_file *)file;
    assert(socket->domain == AF_UNIX);
    assert(socket->type == SOCK_STREAM);
    if (socket->mode != STREAM_CONNECTED) return -ENOTCONN;
    if (flags != 0) return -ETODO;

    size_t w = 0;
    while (true) {
        w += ring_read(&socket->ring, buffer, len);
        wq_notify_all(&socket->write_wq);
        if (w != 0) break;
        wq_block_on(&file->wq);
        if (file->signal_eof) {
            break;
        }
    }

    return w;
}

ssize_t st_socket_send(struct open_file *ofd, const void *buffer, size_t len,
                       int flags) {
    struct file *file = ofd->node;
    assert(file->filetype == FT_SOCKET);
    struct socket_file *socket = (struct socket_file *)file;
    assert(socket->domain == AF_UNIX);
    assert(socket->type == SOCK_STREAM);
    if (flags != 0) return -ETODO;

    struct socket_file *to = socket->pair;

    if (!to) return -ECONNRESET;

    size_t w = 0;
    while (true) {
        w += ring_write(&to->ring, buffer, len);
        wq_notify_all(&to->file.wq);
        if (w == len) break;
        wq_block_on(&to->write_wq);
    }

    return len;
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
    int (*listen)(struct open_file *ofd, int backlog);
    int (*accept)(struct open_file *ofd, struct sockaddr *addr, socklen_t *len);
    int (*connect)(struct open_file *ofd, const struct sockaddr *addr, socklen_t len);
};

struct socket_ops socket_dg_sockops = {
    .recv = dg_socket_recv,
    .recvfrom = dg_socket_recvfrom,
    .sendto = dg_socket_sendto,
};

struct socket_ops socket_st_sockops = {
    .recv = st_socket_recv,
    .send = st_socket_send,
};

struct socket_ops socket_lsn_sockops = {
    .listen = st_socket_listen,
    .accept = st_socket_accept,
    .connect = st_socket_connect,
};

#define GET(fd)                                                                \
    struct open_file *ofd = dmgr_get(&running_process->fds, sock);             \
    if (!ofd) return -EBADF;                                                   \
    struct file *file = ofd->node;                                             \
    if (file->filetype != FT_SOCKET) return -ENOTSOCK;                         \
    struct socket_file *socket = (struct socket_file *)file;


sysret sys_socket(int domain, int type, int protocol) {
    if (domain != AF_UNIX) return -EAFNOSUPPORT;
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
    switch (type) {
    case SOCK_STREAM:
        ring_emplace(&socket->ring, 4096);
        socket->socket_ops = &socket_lsn_sockops;
        break;
    case SOCK_DGRAM: socket->socket_ops = &socket_dg_sockops; break;
    }

    return do_open(&socket->file, NULL, USR_READ | USR_WRITE, 0);
}

sysret sys_bind(int sock, struct sockaddr const *addr, socklen_t addrlen) {
    GET(sock);
    return bind_to_path(socket, (struct sockaddr_un *)addr);
}

sysret sys_connect(int sock, struct sockaddr const *addr, socklen_t addrlen) {
    GET(sock);

    if (socket->socket_ops->connect) {
        return socket->socket_ops->connect(ofd, addr, addrlen);
    } else {
        return -EOPNOTSUPP; // maybe
    }
}

sysret sys_listen(int sock, int backlog) {
    GET(sock);
    if (socket->type != SOCK_STREAM) return -EOPNOTSUPP;

    if (socket->socket_ops->listen) {
        return socket->socket_ops->listen(ofd, backlog);
    } else {
        return -EOPNOTSUPP; // maybe
    }
}

sysret sys_accept(int sock, struct sockaddr *addr, socklen_t *len) {
    GET(sock);
    if (socket->type != SOCK_STREAM) return -EOPNOTSUPP;

    if (socket->socket_ops->accept) {
        return socket->socket_ops->accept(ofd, addr, len);
    } else {
        return -EOPNOTSUPP; // maybe
    }
}

sysret sys_send(int sock, void const *buf, size_t len, int flags) {
    GET(sock);
    if (socket->socket_ops->send) {
        return socket->socket_ops->send(ofd, buf, len, flags);
    } else {
        return -ENOTSOCK;
    }
}

sysret sys_sendto(int sock, void const *buf, size_t len, int flags,
                  struct sockaddr const *remote, socklen_t addrlen) {
    GET(sock);
    if (socket->socket_ops->sendto) {
        return socket->socket_ops->sendto(ofd, buf, len, flags, remote,
                                          addrlen);
    } else {
        return -ENOTSOCK;
    }
}

sysret sys_recv(int sock, void *buf, size_t len, int flags) {
    GET(sock);
    if (socket->socket_ops->recv) {
        return socket->socket_ops->recv(ofd, buf, len, flags);
    } else {
        return -ENOTSOCK;
    }
}

sysret sys_recvfrom(int sock, void *buf, size_t len, int flags,
                    struct sockaddr *remote, socklen_t *addrlen) {
    GET(sock);
    if (socket->socket_ops->recvfrom) {
        return socket->socket_ops->recvfrom(ofd, buf, len, flags, remote,
                                            addrlen);
    } else {
        return -ENOTSOCK;
    }
}
