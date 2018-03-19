
#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <pci.h>
#include <print.h>
#include <arch/x86/cpu.h>
#include "ether.h"
#include "rtl8139.h"

struct mac_addr my_mac;

int init_rtl8139() {
    uint32_t network_card = pci_find_device_by_id(0x10ec, 0x8139);
    printf("Network card ID = ");
    pci_print_addr(network_card);
    printf("\n");
    printf("BAR0: %#010x\n", pci_config_read(network_card + 0x10));
    uint32_t base = pci_config_read(network_card + 0x10);
    /*
    printf("BAR1: %#010x\n", pci_config_read(network_card + 0x14));
    printf("BAR2: %#010x\n", pci_config_read(network_card + 0x18));
    printf("BAR3: %#010x\n", pci_config_read(network_card + 0x1c));
    printf("BAR4: %#010x\n", pci_config_read(network_card + 0x20));
    printf("BAR5: %#010x\n", pci_config_read(network_card + 0x24));
    */

    for (int off=0; off<6; off++) {
        uint8_t c = inb((base & ~1) + off);
        my_mac.data[off] = c;
    }

    print_mac_addr(my_mac);

    return 0;
}

