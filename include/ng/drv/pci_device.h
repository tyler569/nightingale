#pragma once
#ifndef NG_PCI_DEVICE_H
#define NG_PCI_DEVICE_H

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

    [[nodiscard]] constexpr uint32_t addr() const { return m_address; }

    [[nodiscard]] constexpr uint8_t bus() const
    {
        return (m_address >> 16) & 0xFF;
    }

    [[nodiscard]] constexpr uint8_t slot() const
    {
        return (m_address >> 11) & 0x1F;
    }

    [[nodiscard]] constexpr uint8_t func() const
    {
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
        : m_pci_address(pci_address)
    {
    }

    constexpr ~pci_device() = default;

    pci_device(const pci_device &) = delete;
    pci_device(pci_device &&) = delete;
    pci_device &operator=(const pci_device &) = delete;
    pci_device &operator=(pci_device &&) = delete;

    [[nodiscard]] uint8_t pci_read8(int offset) const;
    [[nodiscard]] uint16_t pci_read16(int offset) const;
    [[nodiscard]] uint32_t pci_read(int offset) const;
    void pci_write8(int offset, uint32_t value) const;
    void pci_write16(int offset, uint32_t value) const;
    void pci_write(int offset, uint32_t value) const;

    [[nodiscard]] uint16_t vendor_id() const
    {
        return pci_read16(PCI_VENDOR_ID);
    }
    [[nodiscard]] uint16_t device_id() const
    {
        return pci_read16(PCI_DEVICE_ID);
    }
    [[nodiscard]] uint8_t revision_id() const
    {
        return pci_read8(PCI_REVISION_ID);
    }
    [[nodiscard]] uint8_t interrupt_line() const
    {
        return pci_read8(PCI_INTERRUPT_LINE);
    }
    [[nodiscard]] uint32_t bar0() const { return pci_read(PCI_BAR0); }
    [[nodiscard]] uint32_t bar1() const { return pci_read(PCI_BAR1); }
    [[nodiscard]] uint32_t bar2() const { return pci_read(PCI_BAR2); }
    [[nodiscard]] uint32_t bar3() const { return pci_read(PCI_BAR3); }
    [[nodiscard]] uint32_t bar4() const { return pci_read(PCI_BAR4); }
    [[nodiscard]] uint32_t bar5() const { return pci_read(PCI_BAR5); }
};

nx::optional<pci_address> pci_find_device(
    uint16_t vendor_id, uint16_t device_id);

#endif // NG_PCI_DEVICE_H
