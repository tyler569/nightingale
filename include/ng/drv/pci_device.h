#pragma once

#include <ng/pci.h>
#include <nx/optional.h>
#include <nx/print.h>
#include <stdint.h>
#include <stdio.h>

constexpr uint8_t PCI_VENDOR_ID = 0x00;
constexpr uint8_t PCI_DEVICE_ID = 0x02;
constexpr uint8_t PCI_COMMAND = 0x04;
constexpr uint8_t PCI_STATUS = 0x06;
constexpr uint8_t PCI_REVISION_ID = 0x08;
constexpr uint8_t PCI_PROG_IF = 0x09;
constexpr uint8_t PCI_SUBCLASS = 0x0a;
constexpr uint8_t PCI_CLASS = 0x0b;
constexpr uint8_t PCI_CACHE_LINE_SIZE = 0x0c;
constexpr uint8_t PCI_LATENCY_TIMER = 0x0d;
constexpr uint8_t PCI_HEADER_TYPE = 0x0e;
constexpr uint8_t PCI_BIST = 0x0f;
constexpr uint8_t PCI_BAR0 = 0x10;
constexpr uint8_t PCI_BAR1 = 0x14;
constexpr uint8_t PCI_BAR2 = 0x18;
constexpr uint8_t PCI_BAR3 = 0x1c;
constexpr uint8_t PCI_BAR4 = 0x20;
constexpr uint8_t PCI_BAR5 = 0x24;
constexpr uint8_t PCI_INTERRUPT_LINE = 0x3c;

class pci_address {
	uint32_t m_address;

public:
	explicit constexpr pci_address(uint32_t addr)
		: m_address(addr) { }

	constexpr pci_address(
		uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
		: m_address((bus << 16) | (slot << 11) | (func << 8) | offset) { }

	constexpr pci_address(uint8_t bus, uint8_t slot, uint8_t func)
		: m_address((bus << 16) | (slot << 11) | (func << 8)) { }

	constexpr pci_address(int bus, int slot, int func)
		: m_address((bus << 16) | (slot << 11) | (func << 8)) { }

	[[nodiscard]] constexpr uint32_t addr() const { return m_address; }

	[[nodiscard]] constexpr uint8_t bus() const {
		return (m_address >> 16) & 0xFF;
	}

	[[nodiscard]] constexpr uint8_t slot() const {
		return (m_address >> 11) & 0x1F;
	}

	[[nodiscard]] constexpr uint8_t func() const {
		return (m_address >> 8) & 0x3;
	}

	[[nodiscard]] constexpr uint8_t offset() const { return m_address & 0xFF; }
};

template <> void nx::print(const pci_address &addr);

void pci_write(pci_address pci_address, uint8_t offset, uint32_t value);
uint32_t pci_read(pci_address pci_address, uint8_t offset);

class pci_device {
public:
	pci_address m_pci_address;

	explicit constexpr pci_device(pci_address pci_address)
		: m_pci_address(pci_address) { }

	~pci_device() = default;

	pci_device(const pci_device &) = delete;
	pci_device(pci_device &&) = delete;
	pci_device &operator=(const pci_device &) = delete;
	pci_device &operator=(pci_device &&) = delete;

	[[nodiscard]] uint32_t pci_read(uint8_t offset) const {
		return ::pci_read(m_pci_address, offset);
	}

	void pci_write(uint8_t offset, uint32_t value) const {
		::pci_write(m_pci_address, offset, value);
	}

	[[nodiscard]] uint16_t vendor_id() const { return pci_read(PCI_VENDOR_ID); }

	[[nodiscard]] uint16_t device_id() const { return pci_read(PCI_DEVICE_ID); }

	[[nodiscard]] uint8_t revision_id() const {
		return pci_read(PCI_REVISION_ID);
	}

	[[nodiscard]] uint8_t prog_if() const { return pci_read(PCI_PROG_IF); }

	[[nodiscard]] uint8_t interrupt_line() const {
		return pci_read(PCI_INTERRUPT_LINE);
	}
};

nx::optional<pci_address> pci_find_device(
	uint16_t vendor_id, uint16_t device_id);

