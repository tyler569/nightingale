#include <ng/cpu.h>
#include <ng/init.h>
#include <ng/pci.h>
#include <stdio.h>

pci_address_t pci_pack_addr(int bus, int slot, int func, int offset) {
	return (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xff);
}

void pci_print_addr(pci_address_t pci_addr) {
	if (pci_addr == ~0u) {
		printf("INVALID PCI ID");
		return;
	}

	uint32_t bus = (pci_addr >> 16) & 0xFF;
	uint32_t slot = (pci_addr >> 11) & 0x1F;
	uint32_t func = (pci_addr >> 8) & 0x3;
	uint32_t offset = pci_addr & 0xFF;
	if (offset == 0) {
		printf("%02x:%02x.%x", bus, slot, func);
	} else {
		printf("%02x:%02x.%x+%#02x", bus, slot, func, offset);
	}
}

uint8_t pci_read8(pci_address_t addr, int offset) {
	addr = (addr + offset) | 0x80000000;
	outd(0xCF8, addr);
	return inb(0xCFC);
}

uint16_t pci_read16(pci_address_t addr, int offset) {
	addr = (addr + offset) | 0x80000000;
	outd(0xCF8, addr);
	return inw(0xCFC);
}

uint32_t pci_read32(pci_address_t addr, int offset) {
	addr = (addr + offset) | 0x80000000;
	outd(0xCF8, addr);
	return ind(0xCFC);
}

void pci_write8(pci_address_t addr, int offset, uint8_t value) {
	addr = (addr + offset) | 0x80000000;
	outd(0xCF8, addr);
	outb(0xCFC, value);
}

void pci_write16(pci_address_t addr, int offset, uint16_t value) {
	addr = (addr + offset) | 0x80000000;
	outd(0xCF8, addr);
	outw(0xCFC, value);
}

void pci_write32(pci_address_t addr, int offset, uint32_t value) {
	addr = (addr + offset) | 0x80000000;
	outd(0xCF8, addr);
	outd(0xCFC, value);
}

#define DEFINE_MMIO(T, bits) \
	T pci_mmio_read##bits(uint64_t address, int offset) { \
		uint64_t addr = address + offset; \
		return *(volatile T *)addr; \
	} \
	void pci_mmio_write##bits(uint64_t address, int offset, T value) { \
		uint64_t addr = address + offset; \
		*(volatile T *)addr = value; \
	}

DEFINE_MMIO(uint8_t, 8)
DEFINE_MMIO(uint16_t, 16)
DEFINE_MMIO(uint32_t, 32)
DEFINE_MMIO(uint64_t, 64)

uint32_t pci_get_bar(pci_address_t addr, int bar) {
	if (bar < 0 || bar > 5) {
		return 0;
	}

	uint32_t reg = pci_read32(addr, PCI_BAR0 + bar * 4);
	return reg;
}

void pci_enable_bus_mastering(pci_address_t addr) {
	uint16_t command = pci_read16(addr, PCI_COMMAND);
	pci_write16(addr, PCI_COMMAND, command | 0x04);
}

extern const struct pci_driver *pci_drivers_start[], *pci_drivers_end[];

static bool pci_id_matches(const struct pci_device_id *a, int vendor,
	int device, int class, int subclass) {
	if (a->class || a->subclass)
		return (a->class == class && a->subclass == subclass);
	else
		return a->vendor == vendor && a->device == device;
}

static bool pci_not_end(const struct pci_device_id *a) {
	return a->class || a->subclass || a->vendor || a->device;
}

void pci_probe_device(pci_address_t addr) {
	uint32_t reg = pci_read32(addr, 0);

	if (reg != ~0u) {
		uint16_t ven = reg & 0xFFFF;
		uint16_t dev = reg >> 16;

		reg = pci_read32(addr, 0x08);

		uint8_t class = reg >> 24;
		uint8_t subclass = reg >> 16;
		uint8_t prog_if = reg >> 8;

		printf("pci: found (%04x:%04x) at ", ven, dev);
		pci_print_addr(addr);
		printf("\n");

		for (auto d = pci_drivers_start; d < pci_drivers_end; d++) {
			for (auto id = (*d)->ids; pci_not_end(id); id++) {
				if (!pci_id_matches(id, ven, dev, class, subclass))
					continue;
				if ((*d)->probe(addr, id) == 0)
					return; // otherwise, try other drivers
			}
		}
	}
}

void pci_enumerate_bus_and_print() {
	for (int bus = 0; bus < 256; bus++) {
		for (int slot = 0; slot < 32; slot++) {
			pci_address_t addr = pci_pack_addr(bus, slot, 0, 0);
			if (slot == 0 && pci_read32(addr, 0) == ~0u)
				goto nextbus;

			pci_probe_device(addr);
		}
	nextbus:;
	}
}
define_init(pci_enumerate_bus_and_print, 4);
