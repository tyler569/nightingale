
#pragma once
#ifndef NIGHTINGALE_NET_IF_H
#define NIGHTINGALE_NET_IF_H

#include <basic.h>
#include "rtl8139.h"

enum if_type {
    RTL8139,
};

/*
 * Basically a tagged union of the interface structs
 * each should contain enough information to be able to
 * run all the functions here
 */

struct net_if {
    int type;
    union {
        struct rtl8139_if rtl;
    };
};

bool pending_rx(struct net_if);
// send?
// sockets?

#endif
