#include <ng/cpu.h>
#include <ng/drv/pci.h>
#include <nx/optional.h>
#include <nx/print.h>
#include <stdio.h>

uint8_t pci_read8(pci_address pci_address, int offset)
{
    auto addr = pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    return inb(0xCFC);
}

uint16_t pci_read16(pci_address pci_address, int offset)
{
    auto addr = pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    return inw(0xCFC);
}

uint32_t pci_read32(pci_address pci_address, int offset)
{
    auto addr = pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    return ind(0xCFC);
}

void pci_write8(pci_address pci_address, int offset, uint8_t value)
{
    auto addr = pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    outb(0xCFC, value);
}

void pci_write16(pci_address pci_address, int offset, uint16_t value)
{
    auto addr = pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    outw(0xCFC, value);
}

void pci_write32(pci_address pci_address, int offset, uint32_t value)
{
    auto addr = pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    outd(0xCFC, value);
}

uint8_t mmio_read8(uint64_t address, int offset)
{
    auto addr = address + offset;
    return *(volatile uint8_t *)addr;
}

uint16_t mmio_read16(uint64_t address, int offset)
{
    auto addr = address + offset;
    return *(volatile uint16_t *)addr;
}

uint32_t mmio_read32(uint64_t address, int offset)
{
    auto addr = address + offset;
    return *(volatile uint32_t *)addr;
}

uint64_t mmio_read64(uint64_t address, int offset)
{
    auto addr = address + offset;
    return *(volatile uint64_t *)addr;
}

void mmio_write8(uint64_t address, int offset, uint8_t value)
{
    auto addr = address + offset;
    *(volatile uint8_t *)addr = value;
}

void mmio_write16(uint64_t address, int offset, uint16_t value)
{
    auto addr = address + offset;
    *(volatile uint16_t *)addr = value;
}

void mmio_write32(uint64_t address, int offset, uint32_t value)
{
    auto addr = address + offset;
    *(volatile uint32_t *)addr = value;
}

void mmio_write64(uint64_t address, int offset, uint64_t value)
{
    auto addr = address + offset;
    *(volatile uint64_t *)addr = value;
}

nx::optional<pci_address> pci_find_device(
    uint16_t vendor_id, uint16_t device_id)
{
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            for (int func = 0; func < 8; func++) {
                auto addr = pci_address { bus, slot, func };
                if (pci_read32(addr, 0) == 0xffffffff) {
                    continue;
                }
                if (pci_read32(addr, 0) == (vendor_id | (device_id << 16))) {
                    return addr;
                }
            }
        }
    }
    return nx::nullopt;
}

template <> void nx::print(const pci_address &addr)
{
    printf("%02x:%02x.%x", addr.bus(), addr.slot(), addr.func());
}
