
#pragma once
#ifndef NIGHTINGALE_PCI_H
#define NIGHTINGALE_PCI_H

#include <basic.h>

/*
typedef struct pci_device {
    u32 bus, slot, function;
} PCI_Device;
*/

/* PCI standard config register offsets */
#define PCI_something      0x00
#define PCI_something_2    0x01
#define PCI_something_else 0x02

/*
 * Consider how I will track PCI addresses going forward.
 * there are as I see it 3 options:
 * 1: a custom struct that is { bus, func, slot }
 * 2: the int format PCI addresses are already in (just leave func 0)
 *   -> disadvantage: I will need the actual numbers later for mmio access
 * 3: store a buffer of the devices I care about and index in to that
 *   -> #malloc actually
 */

u32 pci_pack_addr(u32 bus, u32 slot, u32 func, u32 offset);
void pci_print_addr(u32 pci_address);

u32 pci_config_read(u32 pci_address);
void pci_print_device_info(u32 pci_address);
u32 pci_find_device_by_id(u16 vendor, u16 device);

void pci_enumerate_bus_and_print();

const char *pci_device_type(u8 class, u8 subclass, u8 prog_if);

#endif

