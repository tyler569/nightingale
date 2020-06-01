
#include <basic.h>
#include <net/core.h>
#include <net/if.h>
#include <net/socket.h>
#include <ng/fs.h>
#include <stdio.h>

LIST_DEFINE(all_sockets);

sysret sys_socket(int domain, int type, int protocol) {
        struct file *node;
        struct socket_impl *socket;
        struct open_file *ofd;
        int fd;
        
        return -ETODO;

        // return fd;
}
