#include <basic.h>
#include <assert.h>
#include <list.h>
#include <ng/fs.h>
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

static struct socket_ops st_sockops;
static struct socket_ops dg_sockops;
static struct socket_ops listen_sockops;

struct unix_pkb {
    list_node node;
    struct sockaddr_un remote;
    size_t packet_len;
    char packet[];
};

struct dg_file {
    struct socket_file socket;
    struct sockaddr_un address;
    list recv_q;
};

struct st_file {
    struct socket_file socket;
    struct sockaddr_un address;
    struct ringbuf ring;
    struct st_file *pair;
};

static struct file *find_by_sockaddr(struct sockaddr_un *addr, socklen_t len) {
    if (len == sizeof(sa_family_t)) return NULL;
    char *path = addr->sun_path;
    return fs_resolve_relative_path(running_thread->cwd, path);
}

static int dg_bind(struct socket_file *sock, const struct sockaddr *addr,
                      socklen_t len) {
    if (sock->mode != SOCKET_IDLE)
        return -EINVAL; // TODO this is probably wrong
    struct dg_file *dg = (struct dg_file *)sock;
    struct sockaddr_un *path = (struct sockaddr_un *)addr;
    struct file *dir =
        fs_resolve_directory_of(running_thread->cwd, path->sun_path);
    // TODO: EADDRINUSE if the file resolves
    sock->file.refcnt++;
    dg->address = *path;
    sock->mode = SOCKET_BOUND;
    return add_dir_file(dir, &sock->file, strdup(basename(path->sun_path)));
}

static int st_bind(struct socket_file *sock, const struct sockaddr *addr,
                      socklen_t len) {
    if (sock->mode != SOCKET_IDLE)
        return -EINVAL; // TODO this is probably wrong
    struct st_file *st = (struct st_file *)sock;
    struct sockaddr_un *path = (struct sockaddr_un *)addr;
    struct file *dir =
        fs_resolve_directory_of(running_thread->cwd, path->sun_path);
    // TODO: EADDRINUSE if the file resolves
    sock->file.refcnt++;
    st->address = *path;
    sock->mode = SOCKET_BOUND;
    return add_dir_file(dir, &sock->file, strdup(basename(path->sun_path)));
}

struct socket_file *dg_alloc() {
    return zmalloc(sizeof(struct dg_file));
}

void dg_init(struct socket_file *socket) {
    struct dg_file *dg = (struct dg_file *)socket;
    list_init(&dg->recv_q);
}

ssize_t dg_recvfrom(struct open_file *ofd, void *buffer, size_t len,
                    int flags, struct sockaddr *remote,
                    socklen_t *remote_len) {
    struct file *file = ofd->file;
    assert(file->type == FT_SOCKET);
    struct socket_file *sock = (struct socket_file *)file;
    if (sock->type != SOCK_DGRAM) return -EOPNOTSUPP;
    struct dg_file *dg = (struct dg_file *)sock;

    // todo interruptable?
    while (list_empty(&dg->recv_q)) { wq_block_on(&file->readq); }

    struct unix_pkb *pkt = list_pop_front(struct unix_pkb, node, &dg->recv_q);
    if (remote) memcpy(remote, &pkt->remote, sizeof(struct sockaddr_un));
    if (remote_len) *remote_len = sizeof(struct sockaddr_un);
    size_t min_len = umin(len, pkt->packet_len);
    memcpy(buffer, pkt->packet, min_len);
    free(pkt);
    return min_len;
}

ssize_t dg_sendto(struct open_file *ofd, const void *buffer, size_t len,
                         int flags, const struct sockaddr *dest,
                         socklen_t dest_len) {
    struct file *file = find_by_sockaddr((struct sockaddr_un *)dest, dest_len);
    if (!file) return -EACCES;
    if (file->type != FT_SOCKET) return -ECONNREFUSED;
    struct socket_file *sock = (struct socket_file *)file;
    if (sock->type != SOCK_DGRAM) return -EOPNOTSUPP;
    struct dg_file *dg = (struct dg_file *)sock;
    struct unix_pkb *pkt = zmalloc(sizeof(struct unix_pkb) + len);
    pkt->remote = dg->address;
    pkt->packet_len = len;
    memcpy(pkt->packet, buffer, len);
    list_append(&dg->recv_q, &pkt->node);
    wq_notify_all(&file->readq);
    return len;
}

ssize_t dg_recv(struct open_file *ofd, void *buffer, size_t len,
                       int flags) {
    return dg_recvfrom(ofd, buffer, len, flags, NULL, NULL);
}

void socket_close(struct open_file *ofd) {
    struct file *file = ofd->file;
    if (--file->refcnt > 1) return;
    struct socket_file *socket = (struct socket_file *)file;
    if (socket->ops->close) socket->ops->close(ofd);
}

void dg_close(struct open_file *ofd) {
    struct file *file = ofd->file;
    struct socket_file *socket = (struct socket_file *)file;
    wq_notify_all(&file->writeq);
    // free(socket);
}

struct socket_file *st_alloc() {
    return zmalloc(sizeof(struct st_file));
}

void st_init(struct socket_file *socket) {
    struct st_file *st = (struct st_file *)socket;
}

void st_close(struct open_file *ofd) {
    struct file *file = ofd->file;
    struct socket_file *socket = (struct socket_file *)file;
    if (socket->pair) {
        assert(socket->pair->pair == socket);
        socket->pair->file.signal_eof = 1;
        wq_notify_all(&socket->pair->file.readq);
        socket->pair->pair = NULL;
    }
    // free anything in the queue
    wq_notify_all(&file->writeq);
    wq_notify_all(&file->readq);
    // ring_free(&socket->ring); // IFF the ring is allocated
    // free(socket);
}

struct inbound_connection {
    list_node node;
    struct socket_file *socket;
};

int st_listen(struct open_file *ofd, int backlog) {
    struct file *file = ofd->file;
    assert(file->type == FT_SOCKET);
    struct socket_file *socket = (struct socket_file *)file;
    assert(socket->domain == AF_UNIX);
    assert(socket->type == SOCK_STREAM);
    if (socket->mode != SOCKET_BOUND) return -ENOTCONN;

    // TODO support backlog
    list_init(&socket->recv_q);
    socket->mode = STREAM_LISTENING;

    return 0;
}

int st_connect(struct open_file *ofd, const struct sockaddr *addr,
                      socklen_t len) {
    struct file *file = ofd->file;
    assert(file->type == FT_SOCKET);
    struct socket_file *socket = (struct socket_file *)file;
    assert(socket->domain == AF_UNIX);
    assert(socket->type == SOCK_STREAM);

    struct file *cfile = find_by_sockaddr((struct sockaddr_un *)addr, len);
    if (!cfile) return -EACCES;
    if (cfile->type != FT_SOCKET) return -ECONNREFUSED;
    struct socket_file *csocket = (struct socket_file *)cfile;
    if (csocket->type != SOCK_STREAM) return -EOPNOTSUPP;

    struct inbound_connection *c = malloc(sizeof(struct inbound_connection));
    c->socket = socket;
    list_append(&csocket->recv_q, &c->node);
    wq_notify_all(&csocket->file.readq);
    wq_block_on(&file->readq); // accept(2) wakes us back up

    return 0;
}

int st_accept(struct open_file *ofd, struct sockaddr *addr,
                     socklen_t *len) {
    struct file *file = ofd->file;
    assert(file->type == FT_SOCKET);
    struct socket_file *socket = (struct socket_file *)file;
    if (socket->type != SOCK_STREAM) return -EOPNOTSUPP;

    while (list_empty(&socket->recv_q)) { wq_block_on(&file->readq); }

    struct inbound_connection *c =
        list_pop_front(struct inbound_connection, node, &socket->recv_q);

    int fd = sys_socket(AF_UNIX, SOCK_STREAM, 0);
    struct open_file *myfd = dmgr_get(&running_process->fds, fd);
    struct file *myfile = myfd->file;
    struct socket_file *mysocket = (struct socket_file *)myfile;

    mysocket->pair = c->socket;
    c->socket->pair = mysocket;
    mysocket->mode = STREAM_CONNECTED;
    c->socket->mode = STREAM_CONNECTED;

    mysocket->ops = &st_sockops;
    c->socket->ops = &st_sockops;

    memcpy(addr, &c->socket->address, *len);
    *len = min(sizeof(struct sockaddr_un), *len);

    wq_notify_all(&c->socket->file.readq); // wake the connector back up

    return fd;
}

ssize_t st_recv(struct open_file *ofd, void *buffer, size_t len,
                       int flags) {
    struct file *file = ofd->file;
    assert(file->type == FT_SOCKET);
    struct socket_file *socket = (struct socket_file *)file;
    assert(socket->domain == AF_UNIX);
    assert(socket->type == SOCK_STREAM);
    if (socket->mode != STREAM_CONNECTED) return -ENOTCONN;
    if (flags != 0) return -ETODO;

    size_t w = 0;
    while (true) {
        if (file->signal_eof) {
            // This file is super EOF -- any subsequent reads should return 0
            // so we won't reset signal_eof here.
            // Maybe this should reset the ->mode? Can you reuse a socket
            // obtained from accept(2) ? That seems pretty bananas.
            break;
        }
        w += ring_read(&socket->ring, buffer, len);
        wq_notify_all(&file->writeq);
        if (w != 0) break;
        wq_block_on(&file->readq);
    }

    return w;
}

ssize_t st_send(struct open_file *ofd, const void *buffer, size_t len,
                       int flags) {
    struct file *file = ofd->file;
    assert(file->type == FT_SOCKET);
    struct socket_file *socket = (struct socket_file *)file;
    assert(socket->domain == AF_UNIX);
    assert(socket->type == SOCK_STREAM);
    if (flags != 0) return -ETODO;
    struct st_file *stream = (struct st_file *)file;

    struct socket_file *to = socket->pair;

    if (!to) return -ECONNRESET;

    size_t w = 0;
    while (true) {
        w += ring_write(&to->ring, buffer, len);
        wq_notify_all(&to->file.readq);
        if (w == len) break;
        wq_block_on(&to->file.writeq);
    }

    return len;
}

static struct file_ops socket_ops = {
    .close = socket_close,
};

static struct socket_ops dg_sockops = {
    .alloc = dg_alloc,
    .init = dg_init,
    .recv = dg_recv,
    .recvfrom = dg_recvfrom,
    .sendto = dg_sendto,
    .close = dg_close,
    .bind = dg_bind,
};

static struct socket_ops st_sockops = {
    .recv = st_recv,
    .send = st_send,
    .close = st_close,
    .bind = st_bind,
};

static struct socket_ops listen_sockops = {
    .alloc = st_alloc,
    .init = st_init,
    .listen = st_listen,
    .accept = st_accept,
    .connect = st_connect,
    .bind = st_bind,
};
