#pragma once

#include <nx/optional.h>
#include <nx/print.h>
#include <stdint.h>
#include <stdio.h>

static constexpr uint8_t PCI_VENDOR_ID = 0x00;
static constexpr uint8_t PCI_DEVICE_ID = 0x02;
static constexpr uint8_t PCI_COMMAND = 0x04;
static constexpr uint8_t PCI_STATUS = 0x06;
static constexpr uint8_t PCI_REVISION_ID = 0x08;
static constexpr uint8_t PCI_PROG_IF = 0x09;
static constexpr uint8_t PCI_SUBCLASS = 0x0a;
static constexpr uint8_t PCI_CLASS = 0x0b;
static constexpr uint8_t PCI_CACHE_LINE_SIZE = 0x0c;
static constexpr uint8_t PCI_LATENCY_TIMER = 0x0d;
static constexpr uint8_t PCI_HEADER_TYPE = 0x0e;
static constexpr uint8_t PCI_BIST = 0x0f;
static constexpr uint8_t PCI_BAR0 = 0x10;
static constexpr uint8_t PCI_BAR1 = 0x14;
static constexpr uint8_t PCI_BAR2 = 0x18;
static constexpr uint8_t PCI_BAR3 = 0x1c;
static constexpr uint8_t PCI_BAR4 = 0x20;
static constexpr uint8_t PCI_BAR5 = 0x24;
static constexpr uint8_t PCI_INTERRUPT_LINE = 0x3c;

class pci_address {
    uint32_t m_address;

public:
    explicit constexpr pci_address(uint32_t addr)
        : m_address(addr)
    {
    }

    constexpr pci_address(int bus, int slot, int func, int offset = 0)
        : m_address((bus << 16) | (slot << 11) | (func << 8) | offset)
    {
    }

    // clang-format off
    [[nodiscard]] constexpr uint32_t addr() const { return m_address; }
    [[nodiscard]] constexpr uint8_t bus() const { return (m_address >> 16) & 0xFF; }
    [[nodiscard]] constexpr uint8_t slot() const { return (m_address >> 11) & 0x1F; }
    [[nodiscard]] constexpr uint8_t func() const { return (m_address >> 8) & 0x3; }
    [[nodiscard]] constexpr uint8_t offset() const { return m_address & 0xFF; }
    // clang-format on
};

template <> void nx::print(const pci_address &arg);

uint8_t pci_read8(pci_address addr, int offset);
uint16_t pci_read16(pci_address addr, int offset);
uint32_t pci_read32(pci_address addr, int offset);
void pci_write8(pci_address addr, int offset, uint8_t value);
void pci_write16(pci_address addr, int offset, uint16_t value);
void pci_write32(pci_address addr, int offset, uint32_t value);

uint8_t mmio_read8(uint64_t address, int offset);
uint16_t mmio_read16(uint64_t address, int offset);
uint32_t mmio_read32(uint64_t address, int offset);
uint64_t mmio_read64(uint64_t address, int offset);
void mmio_write8(uint64_t address, int offset, uint8_t value);
void mmio_write16(uint64_t address, int offset, uint16_t value);
void mmio_write32(uint64_t address, int offset, uint32_t value);
void mmio_write64(uint64_t address, int offset, uint64_t value);

nx::optional<pci_address> pci_find_device(
    uint16_t vendor_id, uint16_t device_id);
void pci_enable_bus_mastering(pci_address addr);

