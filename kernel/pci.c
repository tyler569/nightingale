
#include <basic.h>
#include <debug.h>
#include <term/terminal.h>

#include "pci.h"


u32 pci_config_read(u32 bus, u32 slot, u32 func, u32 offset) {
    u32 pci_address = (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | 0x80000000;
    outd(0xCF8, pci_address);

    u32 value = ind(0xCFC);
    return value;
}

const char *pci_device_type(u8 class, u8 subclass, u8 prog_if) {
    switch (class) {
    case 0x00:
        switch (subclass) {
        case 0x00:
            return "Device built before definition of class field";
        case 0x01:
            return "VGA-Compatible device";
        default:
            return NULL;
        }
    }
    return NULL;
}

void pci_print_device_info(u32 bus, u32 slot, u32 func) {
    u32 reg = pci_config_read(bus, slot, func, 0);

    if (reg != ~0) {
        u16 ven = reg & 0xFFFF;
        u16 dev = reg >> 16;
        printf("bus %i, slot %i, func %i device %x:%x\n", bus, slot, func, ven, dev);

        reg = pci_config_read(bus, slot, func, 0x3c);
        u8 intnum = reg;
        if (intnum != 0) {
            printf("    uses irq %i\n", intnum);
        }

        reg = pci_config_read(bus, slot, func, 0x08);
        u8 class = reg >> 24;
        u8 subclass = reg >> 16;
        u8 prog_if = reg >> 8;
        const char *dev_type = pci_device_type(class, subclass, prog_if);
        if (dev_type != NULL) {
            printf("    device is: %s\n", dev_type);
        } else {
            printf("    device is unknown; class 0x%x, subclass 0x%x\n", class, subclass);
        }
    }
}

void pci_enumerate_bus_and_print(u32 max_bus, u32 max_slot) {
    for (int bus=0; bus<max_bus; bus++) {
        for (int slot=0; slot<max_slot; slot++) {
            for (int func=0; func<8; func++) {
                pci_print_device_info(bus, slot, func);
            }
        }
    }
}

