#include <basic.h>
#include <ng/cpu.h>
#include <ng/debug.h>
#include <ng/pci.h>

// TODO: 80char this file

uint32_t pci_pack_addr(uint32_t bus, uint32_t slot, uint32_t func,
                       uint32_t offset) {
    return (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xff);
}

void pci_print_addr(uint32_t pci_addr) {
    if (pci_addr == ~0) {
        printf("INVALID PCI ID");
        return;
    }

    uint32_t bus = (pci_addr >> 16) & 0xFF;
    uint32_t slot = (pci_addr >> 11) & 0x1F;
    uint32_t func = (pci_addr >> 8) & 0x3;
    uint32_t offset = (pci_addr)&0xFF;
    if (offset == 0) {
        printf("%02x:%02x.%x", bus, slot, func);
    } else {
        printf("%02x:%02x.%x+%#02x", bus, slot, func, offset);
    }
}

uint32_t pci_config_read(uint32_t pci_address) {
    pci_address &= 0xFFFFFFFC;
    pci_address |= 0x80000000;
    outd(0xCF8, pci_address);

    uint32_t value = ind(0xCFC);
    return value;
}

void pci_config_write(uint32_t pci_address, uint32_t value) {
    pci_address &= 0xFFFFFFFC;
    pci_address |= 0x80000000;
    outd(0xCF8, pci_address);

    outd(0xCFC, value);
}

void pci_print_device_info(uint32_t pci_address) {
    uint32_t reg = pci_config_read(pci_address);

    if (reg != ~0) {
        uint16_t ven = reg & 0xFFFF;
        uint16_t dev = reg >> 16;

        reg = pci_config_read(pci_address + 0x08);

        uint8_t class = reg >> 24;
        uint8_t subclass = reg >> 16;
        uint8_t prog_if = reg >> 8;

        const char *dev_type = pci_device_type(class, subclass, prog_if);

        printf("pci: found %s (%04x:%04x) at ", dev_type, ven, dev);
        pci_print_addr(pci_address);
        printf("\n");
    }
}

void pci_enumerate_bus_and_print() {
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            for (int func = 0; func < 8; func++) {
                uint32_t address = pci_pack_addr(bus, slot, func, 0);
                if (slot == 0 && func == 0 && pci_config_read(address) == -1) {
                    goto nextbus;
                }

                pci_print_device_info(address);
            }
        }
    nextbus:;
    }
}

/*
 * Generall obsoleted by pci_device_callback, uses should be moved over
 */
uint32_t pci_find_device_by_id(uint16_t vendor, uint16_t device) {
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            for (int func = 0; func < 8; func++) {
                uint32_t reg =
                    pci_config_read(pci_pack_addr(bus, slot, func, 0));
                if (slot == 0 && func == 0 && reg == ~0) goto nextbus;
                if (reg == ~0) continue;

                uint16_t ven = reg & 0xFFFF;
                uint16_t dev = reg >> 16;

                if (vendor == ven && device == dev) {
                    return pci_pack_addr(bus, slot, func, 0);
                }
            } // func
        }     // slot
    nextbus:;
    } // bus
    return -1;
}

/*
 * Intended for cases where you want to initialize all of a certain device
 * type, such as all network interfaces.
 */
void pci_device_callback(uint16_t vendor, uint16_t device,
                         void (*callback)(uint32_t)) {
    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            for (int func = 0; func < 8; func++) {
                uint32_t addr = pci_pack_addr(bus, slot, func, 0);

                uint32_t reg = pci_config_read(addr);
                if (slot == 0 && func == 0 && reg == ~0) goto nextbus;

                if (reg == ~0) continue;

                uint16_t ven = reg & 0xFFFF;
                uint16_t dev = reg >> 16;

                if (vendor == ven && device == dev) callback(addr);
            } // func
        }     // slot
    nextbus:;
    } // bus
}

const char *pci_device_type(uint8_t class, uint8_t subclass, uint8_t prog_if) {
    return "";
}
