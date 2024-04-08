#pragma once

#include <netinet/in.h/in.h>
#include <ng/fs/types.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct socket_operations {
	int (*bind)(struct inode *inode, struct sockaddr *addr, socklen_t addrlen);
	int (*listen)(struct inode *inode, int backlog);
	int (*accept)(
		struct inode *inode, struct sockaddr *addr, socklen_t *addrlen);
	int (*connect)(
		struct inode *inode, struct sockaddr *addr, socklen_t addrlen);
	int (*send)(struct inode *inode, const void *buf, size_t len, int flags);
	int (*recv)(struct inode *inode, void *buf, size_t len, int flags);
};

struct inode *new_socket(void);

END_DECLS
