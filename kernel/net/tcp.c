
#include <basic.h>
#include <stdint.h>
#include "inet.h"
#include "tcp.h"

static int open_connection() {
    // send syn
    // receive syn/ack
    // send ack
    return 0;
}

static int listen() {
    // (later)
    // listen for any local port
    // receive syn
    // send syn/ack
    // receive ack
    // make new conneciton and return it
    return 0;
}

/*
 * integrates with
 * sys_socket
 * sys_accept
 * sys_bind
 * sys_connect
 * 
 * in ways that are quite different from udp
 */
