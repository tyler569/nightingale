#pragma once

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

enum socket_mode {
	SOCKET_IDLE,
	SOCKET_BOUND,
	STREAM_LISTENING,
	STREAM_CONNECTED,
	STREAM_CLOSED,
};

enum socket_domain {
	AF_UNIX,
	AF_LOCAL = AF_UNIX,
	AF_INET,
	AF_INET6,
};

enum socket_type {
	SOCK_DGRAM,
	SOCK_STREAM,
	SOCK_RAW,
	SOCK_SEQPACKET,
	SOCK_RDM,
	SOCK_PACKET,
};

typedef size_t socklen_t;

#ifndef __kernel__
int socket(int domain, int type, int protocol);
int bind(int sockfd, struct sockaddr const *addr, socklen_t addrlen);

int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int connect(int sock, struct sockaddr const *addr, socklen_t addrlen);

ssize_t send(int sock, void const *buf, size_t len, int flags);
ssize_t sendto(int sock, void const *buf, size_t len, int flags,
	struct sockaddr const *remote, socklen_t addrlen);
ssize_t recv(int sock, void *buf, size_t len, int flags);
ssize_t recvfrom(int sock, void *buf, size_t len, int flags,
	struct sockaddr *remote, socklen_t *addrlen);
#endif // ifndef __kernel__

END_DECLS

