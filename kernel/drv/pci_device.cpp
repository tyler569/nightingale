#include <ng/cpu.h>
#include <ng/drv/pci_device.h>
#include <nx/optional.h>
#include <nx/print.h>
#include <stdio.h>

uint8_t pci_device::pci_read8(int offset) const
{
    auto addr = m_pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    return inb(0xCFC);
}

uint16_t pci_device::pci_read16(int offset) const
{
    auto addr = m_pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    return inw(0xCFC);
}

uint32_t pci_device::pci_read(int offset) const
{
    auto addr = m_pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    return ind(0xCFC);
}

void pci_device::pci_write8(int offset, uint32_t value) const
{
    auto addr = m_pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    outb(0xCFC, value);
}

void pci_device::pci_write16(int offset, uint32_t value) const
{
    auto addr = m_pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    outw(0xCFC, value);
}

void pci_device::pci_write(int offset, uint32_t value) const
{
    auto addr = m_pci_address.addr() + offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    outd(0xCFC, value);
}

nx::optional<pci_address> pci_find_device(
    uint16_t vendor_id, uint16_t device_id)
{
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            for (int func = 0; func < 8; func++) {
                auto addr = pci_address { bus, slot, func };
                auto dev = pci_device { addr };

                if (dev.device_id() == device_id
                    && dev.vendor_id() == vendor_id) {
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
