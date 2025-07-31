#pragma once

#include <netinet/in.h>
#include <ng/fs/types.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct socket_ops {
	int (*bind)(struct vnode *vnode, struct sockaddr *addr, socklen_t addrlen);
	int (*listen)(struct vnode *vnode, int backlog);
	int (*accept)(
		struct vnode *vnode, struct sockaddr *addr, socklen_t *addrlen);
	int (*connect)(
		struct vnode *vnode, struct sockaddr *addr, socklen_t addrlen);
	int (*send)(struct vnode *vnode, const void *buf, size_t len, int flags);
	int (*recv)(struct vnode *vnode, void *buf, size_t len, int flags);
};

struct vnode *new_socket();

END_DECLS
