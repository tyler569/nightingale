#include <ng/cpu.h>
#include <ng/drv/pci_device.h>
#include <ng/pci.h>
#include <nx/optional.h>

void pci_write(pci_address pci_address, uint8_t offset, uint32_t value) noexcept
{
    outd(0xCF8, pci_address.addr() | offset);
    outd(0xCFC, value);
}

uint32_t pci_read(pci_address pci_address, uint8_t offset) noexcept
{
    outd(0xCF8, pci_address.addr() | offset);
    return ind(0xCFC);
}

nx::optional<pci_address> pci_find_device(
    uint16_t vendor_id, uint16_t device_id) noexcept
{
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            pci_address addr = { (uint8_t)bus, (uint8_t)slot, 0 };
            if (pci_read(addr, 0) == 0xffffffff) {
                continue;
            }
            if (pci_read(addr, 0) == (vendor_id | (device_id << 16))) {
                return addr;
            }
        }
    }
    return nx::nullopt;
}