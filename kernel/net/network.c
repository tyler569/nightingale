
#include <basic.h>
#include <stdint.h>

#include <print.h>
#include <pci.h>

#include "socket.h"
#include "rtl8139.h"

void network_init(void) {

    // 
    // find a network card
    // init it
    // init sockets with that card
    //
    
    uint32_t rtl_addr = pci_find_device_by_id(0x10ec, 0x8139);
    if (rtl_addr == 0) {
        printf("network: init failed, no nic found\n");
        return;
    }
    struct net_if* rtl = init_rtl8139(rtl_addr);

    sockets_init(rtl);
}
