
#pragma once
#ifndef NIGHTINGALE_PCI_H
#define NIGHTINGALE_PCI_H

#include <basic.h>

/*
typedef struct pci_device {
    u32 bus, slot, function;
} PCI_Device;
*/

u32 pci_pack_addr(u32 bus, u32 slot, u32 func, u32 offset);
void pci_print_addr(u32 pci_address);

u32 pci_config_read(u32 pci_address);
void pci_print_device_info(u32 pci_address);
u32 pci_find_device_by_id(u16 vendor, u16 device);

void pci_enumerate_bus_and_print_x(u32 max_bus, u32 max_slot);
void pci_enumerate_bus_and_print();

const char *pci_device_type(u8 class, u8 subclass, u8 prog_if);

#endif

