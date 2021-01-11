#include <basic.h>
#include <assert.h>
#include <list.h>
#include <ng/fs.h>
#include <ng/fs/socket.h>
#include <ng/ringbuf.h>
#include <ng/thread.h>
#include <sys/socket.h>
#include <sys/un.h>

struct protocol_ops {
    enum socket_domain domain;
    enum socket_type type;
    enum socket_protocol protocol;

    struct socket_ops *ops;
};

#define NPROTOCOLS 32
struct protocol_ops protocols[NPROTOCOLS] = {0};

void register_protocol(int domain, int type, int protocol,
                       struct socket_ops *ops) {
    for (int i = 0; i < NPROTOCOLS; i++) {
        if (!protocols[i].ops) {
            protocols[i].domain = domain;
            protocols[i].type = type;
            protocols[i].protocol = protocol;
            protocols[i].ops = ops;
            return;
        }
    }
}

struct socket_ops *get_protocol(int domain, int type, int protocol) {
    for (int i = 0; i < NPROTOCOLS; i++) {
        struct protocol_ops p = protocols[i];
        if (p.domain == domain && p.type == type &&
            (p.protocol == protocol || protocol == PROTO_DEFAULT)) {

            return p.ops;
        }
    }
    return NULL;
}

void socket_close(struct open_file *ofd) {
    struct file *file = ofd->file;
    if (--file->refcnt > 1) return;
    struct socket_file *socket = (struct socket_file *)file;
    if (socket->ops->close) socket->ops->close(ofd);
}

struct file_ops socket_ops = {
    .close = socket_close,
};

#define GET(fd)                                                                \
    struct open_file *ofd = dmgr_get(&running_process->fds, sock);             \
    if (!ofd) return -EBADF;                                                   \
    struct file *file = ofd->file;                                             \
    if (file->type != FT_SOCKET) return -ENOTSOCK;                             \
    struct socket_file *socket = (struct socket_file *)file;


sysret sys_socket(int domain, int type, int protocol) {
    struct socket_ops *ops = get_protocol(domain, type, protocol);
    if (!ops) return -EPROTONOSUPPORT;

    struct socket_file *socket = ops->alloc();
    socket->file.type = FT_SOCKET;
    socket->file.ops = &socket_ops;
    socket->file.mode = USR_READ | USR_WRITE;
    socket->mode = SOCKET_IDLE;
    socket->domain = domain;
    socket->type = type;
    socket->protocol = protocol;
    socket->ops = ops;
    wq_init(&socket->file.readq);
    wq_init(&socket->file.writeq);

    if (socket->ops->init) socket->ops->init(socket);

    return do_open(&socket->file, NULL, USR_READ | USR_WRITE, 0);
}

sysret sys_bind(int sock, struct sockaddr const *addr, socklen_t addrlen) {
    GET(sock);
    if (!socket->ops->bind) return -EOPNOTSUPP;
    return socket->ops->bind(ofd, addr, addrlen);
}

sysret sys_connect(int sock, struct sockaddr const *addr, socklen_t addrlen) {
    GET(sock);
    if (!socket->ops->connect) return -EOPNOTSUPP;
    return socket->ops->connect(ofd, addr, addrlen);
}

sysret sys_listen(int sock, int backlog) {
    GET(sock);
    if (socket->type != SOCK_STREAM) return -EOPNOTSUPP;
    if (socket->ops->listen) return -EOPNOTSUPP;
    return socket->ops->listen(ofd, backlog);
}

sysret sys_accept(int sock, struct sockaddr *addr, socklen_t *len) {
    GET(sock);
    if (socket->type != SOCK_STREAM) return -EOPNOTSUPP;
    if (!socket->ops->accept) return -EOPNOTSUPP;
    return socket->ops->accept(ofd, addr, len);
}

sysret sys_send(int sock, void const *buf, size_t len, int flags) {
    GET(sock);
    if (!socket->ops->send) return -EOPNOTSUPP;
    return socket->ops->send(ofd, buf, len, flags);
}

sysret sys_sendto(int sock, void const *buf, size_t len, int flags,
                  struct sockaddr const *remote, socklen_t addrlen) {
    GET(sock);
    if (socket->ops->sendto) return -EOPNOTSUPP;
    return socket->ops->sendto(ofd, buf, len, flags, remote, addrlen);
}

sysret sys_recv(int sock, void *buf, size_t len, int flags) {
    GET(sock);
    if (socket->ops->recv) return -EOPNOTSUPP;
    return socket->ops->recv(ofd, buf, len, flags);
}

sysret sys_recvfrom(int sock, void *buf, size_t len, int flags,
                    struct sockaddr *remote, socklen_t *addrlen) {
    GET(sock);
    if (socket->ops->recvfrom) return -EOPNOTSUPP;
    return socket->ops->recvfrom(ofd, buf, len, flags, remote, addrlen);
}
