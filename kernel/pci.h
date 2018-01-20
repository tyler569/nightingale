
#pragma once
#ifndef NIGHTINGALE_PCI_H
#define NIGHTINGALE_PCI_H

#include <basic.h>


u32 pci_config_read(u32 bus, u32 slot, u32 func, u32 offset);
const char *pci_device_type(u8 class, u8 subclass, u8 prog_if);
void pci_print_device_info(u32 bus, u32 slot, u32 func);
void pci_enumerate_bus_and_print(u32 max_bus, u32 max_slot);

#endif

