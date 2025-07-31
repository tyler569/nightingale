#include <arpa/inet.h>
#include <netinet/hdr.h>
#include <netinet/in.h>
#include <ng/fs/socket.h>
#include <ng/fs/vnode.h>
#include <ng/net.h>
#include <ng/pk.h>
#include <ng/sync.h>
#include <ng/thread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

struct inet_socket {
	enum socket_mode mode;
	enum socket_type type;

	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;

	// Receive queue
	struct pk *recv_head;
	struct pk *recv_tail;
	spinlock_t recv_lock;

	// Send queue for TCP
	struct pk *send_head;
	struct pk *send_tail;
	spinlock_t send_lock;

	// TCP state
	uint32_t seq_num;
	uint32_t ack_num;
	uint16_t window;

	// UDP/TCP port binding
	uint16_t bound_port;
};

static int inet_bind(
	struct vnode *vnode, struct sockaddr *addr, socklen_t addrlen);
static int inet_listen(struct vnode *vnode, int backlog);
static int inet_accept(
	struct vnode *vnode, struct sockaddr *addr, socklen_t *addrlen);
static int inet_connect(
	struct vnode *vnode, struct sockaddr *addr, socklen_t addrlen);
static int inet_send(
	struct vnode *vnode, const void *buf, size_t len, int flags);
static int inet_recv(struct vnode *vnode, void *buf, size_t len, int flags);

static struct socket_ops inet_socket_ops = {
	.bind = inet_bind,
	.listen = inet_listen,
	.accept = inet_accept,
	.connect = inet_connect,
	.send = inet_send,
	.recv = inet_recv,
};

// Global port allocation
static uint16_t next_ephemeral_port = 32768;
static spinlock_t port_lock = {};

// Socket registry for port lookups
#define MAX_SOCKETS 1024
static struct vnode *socket_registry[MAX_SOCKETS];
static spinlock_t registry_lock = {};

static uint16_t allocate_port() {
	spin_lock(&port_lock);
	uint16_t port = next_ephemeral_port++;
	if (next_ephemeral_port > 65535) {
		next_ephemeral_port = 32768;
	}
	spin_unlock(&port_lock);
	return port;
}

static int register_socket(struct vnode *vnode, uint16_t port) {
	spin_lock(&registry_lock);
	if (port >= MAX_SOCKETS) {
		spin_unlock(&registry_lock);
		return -1;
	}
	if (socket_registry[port] != nullptr) {
		spin_unlock(&registry_lock);
		return -1; // Port already in use
	}
	socket_registry[port] = vnode;
	spin_unlock(&registry_lock);
	return 0;
}

static void __attribute__((unused)) unregister_socket(uint16_t port) {
	spin_lock(&registry_lock);
	if (port < MAX_SOCKETS) {
		socket_registry[port] = nullptr;
	}
	spin_unlock(&registry_lock);
}

struct vnode *find_socket_by_port(uint16_t port) {
	spin_lock(&registry_lock);
	struct vnode *result = nullptr;
	if (port < MAX_SOCKETS) {
		result = socket_registry[port];
	}
	spin_unlock(&registry_lock);
	return result;
}

struct vnode *new_inet_socket(enum socket_type type) {
	extern struct file_system *initfs_file_system;
	struct vnode *vnode = new_vnode(initfs_file_system, _NG_SOCK);
	if (!vnode)
		return nullptr;

	struct inet_socket *sock = malloc(sizeof(struct inet_socket));
	if (!sock) {
		// TODO: free vnode
		return nullptr;
	}

	memset(sock, 0, sizeof(struct inet_socket));
	sock->mode = SOCKET_IDLE;
	sock->type = type;

	vnode->data = sock;
	vnode->socket_ops = &inet_socket_ops;

	return vnode;
}

static int inet_bind(
	struct vnode *vnode, struct sockaddr *addr, socklen_t addrlen) {
	struct inet_socket *sock = (struct inet_socket *)vnode->data;

	if (sock->mode != SOCKET_IDLE) {
		return -EINVAL;
	}

	if (addr->sa_family != AF_INET || addrlen < sizeof(struct sockaddr_in)) {
		return -EINVAL;
	}

	struct sockaddr_in *in_addr = (struct sockaddr_in *)addr;
	uint16_t port = ntohs(in_addr->sin_port);

	if (port == 0) {
		port = allocate_port();
		in_addr->sin_port = htons(port);
	}

	if (register_socket(vnode, port) < 0) {
		return -EADDRINUSE;
	}

	sock->local_addr = *in_addr;
	sock->bound_port = port;
	sock->mode = SOCKET_BOUND;

	return 0;
}

static int inet_listen(struct vnode *vnode, int backlog) {
	struct inet_socket *sock = (struct inet_socket *)vnode->data;

	if (sock->mode != SOCKET_BOUND || sock->type != SOCK_STREAM) {
		return -EINVAL;
	}

	sock->mode = STREAM_LISTENING;
	return 0;
}

static int inet_accept(
	struct vnode *vnode, struct sockaddr *addr, socklen_t *addrlen) {
	// TODO: Implement TCP accept
	return -ENOSYS;
}

static int inet_connect(
	struct vnode *vnode, struct sockaddr *addr, socklen_t addrlen) {
	// TODO: Implement TCP connect
	return -ENOSYS;
}

static int inet_send(
	struct vnode *vnode, const void *buf, size_t len, int flags) {
	struct inet_socket *sock = (struct inet_socket *)vnode->data;

	if (sock->type == SOCK_DGRAM) {
		// UDP send - need destination from sendto()
		return -ENOTCONN;
	} else if (sock->type == SOCK_STREAM) {
		// TCP send
		return -ENOSYS;
	}

	return -EINVAL;
}

int udp_send(struct sockaddr_in *src, struct sockaddr_in *dest,
	const void *data, size_t data_len);

int inet_sendto(struct vnode *vnode, const void *buf, size_t len, int flags,
	struct sockaddr *dest_addr, socklen_t addrlen) {
	struct inet_socket *sock = (struct inet_socket *)vnode->data;

	if (sock->type != SOCK_DGRAM) {
		return -EINVAL;
	}

	if (dest_addr->sa_family != AF_INET
		|| addrlen < sizeof(struct sockaddr_in)) {
		return -EINVAL;
	}

	struct sockaddr_in *dest = (struct sockaddr_in *)dest_addr;

	// If not bound, bind to ephemeral port
	if (sock->mode == SOCKET_IDLE) {
		sock->local_addr.sin_family = AF_INET;
		sock->local_addr.sin_addr.s_addr = INADDR_ANY;
		sock->local_addr.sin_port = htons(allocate_port());
		sock->bound_port = ntohs(sock->local_addr.sin_port);

		if (register_socket(vnode, sock->bound_port) < 0) {
			return -EADDRINUSE;
		}
		sock->mode = SOCKET_BOUND;
	}

	return udp_send(&sock->local_addr, dest, buf, len);
}

static int inet_recv(struct vnode *vnode, void *buf, size_t len, int flags) {
	struct inet_socket *sock = (struct inet_socket *)vnode->data;

	spin_lock(&sock->recv_lock);

	if (!sock->recv_head) {
		spin_unlock(&sock->recv_lock);
		return -EAGAIN; // No data available
	}

	struct pk *pk = sock->recv_head;
	sock->recv_head = pk->queue_next;
	if (!sock->recv_head) {
		sock->recv_tail = nullptr;
	}

	spin_unlock(&sock->recv_lock);

	// Extract data from packet
	size_t data_len = pk->len - pk->l4_offset - sizeof(struct udp_hdr);
	if (data_len > len) {
		data_len = len;
	}

	uint8_t *data = pk->data + pk->l4_offset + sizeof(struct udp_hdr);
	memcpy(buf, data, data_len);

	pk_free(pk);
	return data_len;
}

void inet_socket_deliver(struct vnode *vnode, struct pk *pk) {
	struct inet_socket *sock = (struct inet_socket *)vnode->data;

	spin_lock(&sock->recv_lock);

	if (!sock->recv_head) {
		sock->recv_head = pk;
		sock->recv_tail = pk;
	} else {
		sock->recv_tail->queue_next = pk;
		sock->recv_tail = pk;
	}
	pk->queue_next = nullptr;

	spin_unlock(&sock->recv_lock);
}