#include <ng/cpu.h>
#include <ng/drv/pci_device.h>
#include <nx/functional.h>
#include <nx/optional.h>
#include <nx/print.h>
#include <stdio.h>

void pci_reg_write(pci_address pci_address, uint8_t offset, uint32_t value)
{
    auto addr = pci_address.addr() | offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    outd(0xCFC, value);
}

uint32_t pci_reg_read(pci_address pci_address, uint8_t offset)
{
    auto addr = pci_address.addr() | offset;
    addr |= 0x80000000;
    outd(0xCF8, addr);
    return ind(0xCFC);
}

std::optional<pci_address> pci_find_device(
    uint16_t vendor_id, uint16_t device_id)
{
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            for (int func = 0; func < 8; func++) {
                auto addr = pci_address { bus, slot, func };
                if (pci_reg_read(addr, 0) == 0xffffffff) {
                    continue;
                }
                if (pci_reg_read(addr, 0) == (vendor_id | (device_id << 16))) {
                    return addr;
                }
            }
        }
    }
    return std::nullopt;
}

template <> void nx::print(const pci_address &arg)
{
    printf("%02x:%02x.%x", arg.bus(), arg.slot(), arg.func());
}
