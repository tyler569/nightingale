#pragma once

#include <ng/fs/vnode.h>
#include <ng/pk.h>
#include <sys/cdefs.h>
#include <sys/socket.h>

BEGIN_DECLS

struct vnode *new_inet_socket(enum socket_type type);
struct vnode *find_socket_by_port(uint16_t port);
void inet_socket_deliver(struct vnode *vnode, struct pk *pk);

END_DECLS