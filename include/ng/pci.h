#pragma once

#include <stdint.h>
#include <sys/cdefs.h>

/*
 * Consider how I will track PCI addresses going forward.
 * there are as I see it 3 options:
 * 1: a custom struct that is { bus, func, slot }
 * 2: the int format PCI addresses are already in (just leave func 0)
 *   -> disadvantage: I will need the actual numbers later for mmio access
 * 3: store a buffer of the devices I care about and index in to that
 *   -> #malloc actually
 */

BEGIN_DECLS

typedef uint32_t pci_address_t;

enum {
	PCI_VENDOR_ID = 0x00,
	PCI_DEVICE_ID = 0x02,
	PCI_COMMAND = 0x04,
	PCI_STATUS = 0x06,
	PCI_REVISION = 0x08,
	PCI_PROG_IF = 0x09,
	PCI_SUBCLASS = 0x0A,
	PCI_CLASS = 0x0B,
	PCI_CACHE_LINE_SIZE = 0x0C,
	PCI_LATENCY_TIMER = 0x0D,
	PCI_HEADER_TYPE = 0x0E,
	PCI_BIST = 0x0F,
	PCI_BAR0 = 0x10,
	PCI_BAR1 = 0x14,
	PCI_BAR2 = 0x18,
	PCI_BAR3 = 0x1C,
	PCI_BAR4 = 0x20,
	PCI_BAR5 = 0x24,
	PCI_INTERRUPT_LINE = 0x3C,
	PCI_INTERRUPT_PIN = 0x3D,
};

pci_address_t pci_pack_addr(int bus, int slot, int func, int offset);
void pci_print_addr(pci_address_t);

uint8_t pci_read8(pci_address_t, int offset);
void pci_write8(pci_address_t, int offset, uint8_t value);
uint16_t pci_read16(pci_address_t, int offset);
void pci_write16(pci_address_t, int offset, uint16_t value);
uint32_t pci_read32(pci_address_t, int offset);
void pci_write32(pci_address_t, int offset, uint32_t value);

uint8_t pci_mmio_read8(uint64_t, int offset);
void pci_mmio_write8(uint64_t, int offset, uint8_t value);
uint16_t pci_mmio_read16(uint64_t, int offset);
void pci_mmio_write16(uint64_t, int offset, uint16_t value);
uint32_t pci_mmio_read32(uint64_t, int offset);
void pci_mmio_write32(uint64_t, int offset, uint32_t value);
uint64_t pci_mmio_read64(uint64_t, int offset);
void pci_mmio_write64(uint64_t, int offset, uint64_t value);

void pci_enable_bus_mastering(pci_address_t);

void pci_print_device_info(pci_address_t);
pci_address_t pci_find_device_by_id(uint16_t vendor, uint16_t device);
void pci_device_callback(
	uint16_t vendor, uint16_t device, void (*)(pci_address_t));

void pci_enumerate_bus_and_print();

const char *pci_device_type(
	unsigned char cls, unsigned char subcls, unsigned char prog_if);

END_DECLS
