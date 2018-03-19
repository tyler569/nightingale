
#include <basic.h>
#include <stdint.h>
#include <stddef.h>
#include <print.h>
#include "ether.h"

size_t print_mac_addr(struct mac_addr mac) {
    return printf("%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", 
            mac.data[0], mac.data[1], mac.data[2],
            mac.data[3], mac.data[4], mac.data[5]);
}

