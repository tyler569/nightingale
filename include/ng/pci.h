
#pragma once
#ifndef NIGHTINGALE_PCI_H
#define NIGHTINGALE_PCI_H

#include <ng/basic.h>

/*
typedef struct pci_device {
    uint32_t bus, slot, function;
} PCI_Device;
*/

/* PCI standard config register offsets */
#define PCI_something 0x00
#define PCI_something_2 0x01
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

uint32_t pci_pack_addr(uint32_t bus, uint32_t slot, uint32_t func,
                       uint32_t offset);
void pci_print_addr(uint32_t pci_address);

uint32_t pci_config_read(uint32_t pci_address);
void pci_config_write(uint32_t pci_address, uint32_t value);
void pci_print_device_info(uint32_t pci_address);
uint32_t pci_find_device_by_id(uint16_t vendor, uint16_t device);

void pci_enumerate_bus_and_print();

const char *pci_device_type(unsigned char class, unsigned char subclass,
                            unsigned char prog_if);

#endif
