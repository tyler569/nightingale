
#include <basic.h>
#include <debug.h>

#include "cpu/portio.h"
#include "pci.h"

u32 pci_pack_addr(u32 bus, u32 slot, u32 func, u32 offset) {
    return (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xff);
}

void pci_print_addr(u32 pci_addr) {
    if (pci_addr == ~0) {
        printf("INVALID PCI ID");
        return;
    }

    u32 bus = (pci_addr >> 16) & 0xFF;
    u32 slot = (pci_addr >> 11) & 0x1F;
    u32 func = (pci_addr >> 8) & 0x3;
    u32 offset = (pci_addr) & 0xFF;
    if (offset == 0) {
        printf("%02x:%02x.%x", bus, slot, func);
    } else {
        printf("%02x:%02x.%x+%#02x", bus, slot, func, offset);
    }
}

u32 pci_config_read(u32 pci_address) {
    pci_address &= 0xFFFFFFFC;
    pci_address |= 0x80000000;
    outd(0xCF8, pci_address);

    u32 value = ind(0xCFC);
    return value;
}

void pci_print_device_info(u32 pci_address) {
    u32 reg = pci_config_read(pci_address);

    if (reg != ~0) {
        u16 ven = reg & 0xFFFF;
        u16 dev = reg >> 16;

        pci_print_addr(pci_address);
        printf(": device %04x:%04x\n", ven, dev);

        reg = pci_config_read(pci_address + 0x3c);
        u8 intnum = reg;
        if (intnum != 0) {
            printf("    uses irq %i\n", intnum);
        }

        reg = pci_config_read(pci_address + 0x08);

        u8 class = reg >> 24;
        u8 subclass = reg >> 16;
        u8 prog_if = reg >> 8;

        const char *dev_type = pci_device_type(class, subclass, prog_if);
        if (dev_type != NULL) {
            printf("    device is: %s\n", dev_type);
        } else {
            printf("    device is unknown; class %#x, subclass %#x\n", class, subclass);
        }
    }
}

/*
void pci_enumerate_bus_and_print_x(u32 max_bus, u32 max_slot) {
    for (int bus=0; bus<max_bus; bus++) {
        for (int slot=0; slot<max_slot; slot++) {
            for (int func=0; func<8; func++) {
                pci_print_device_info(pci_pack_addr(bus, slot, func, 0));
            }
        }
    }
}
*/

void pci_enumerate_bus_and_print() {
    for (int bus=0; bus<256; bus++) {
        for (int slot=0; slot<32; slot++) {
            for (int func=0; func<8; func++) {
                u32 address = pci_pack_addr(bus, slot, func, 0);
                if (slot == 0 && func == 0 && pci_config_read(address) == -1)
                    goto nextbus;

                pci_print_device_info(address);
            }
        }
nextbus: ;
    }
}

u32 pci_find_device_by_id(u16 vendor, u16 device) {
    for (int bus=0; bus<256; bus++) {
        for (int slot=0; slot<32; slot++) {
            for (int func=0; func<8; func++) {

                u32 reg = pci_config_read(pci_pack_addr(bus, slot, func, 0));
                if (slot == 0 && func == 0 && reg == -1)
                    goto nextbus;


                if (reg == ~0) {
                    continue;
                }

                u16 ven = reg & 0xFFFF;
                u16 dev = reg >> 16;

                if (vendor == ven && device == dev) {
                    return pci_pack_addr(bus, slot, func, 0);
                }
            }
        }
nextbus: ;
    }

    return -1;
}

const char *pci_device_type(u8 class, u8 subclass, u8 prog_if) {
    switch (class) {
        case 0x00:
            switch (subclass) {
                case 0x00: return "Non-VGA unclassified device";
                case 0x01: return "VGA compatible unclassified device";
                default: return "Unclassified device";
            }
        case 0x01:
            switch (subclass) {
                case 0x00: return "SCSI storage controller";
                case 0x01: return "IDE interface";
                case 0x02: return "Floppy disk controller";
                case 0x03: return "IPI bus controller";
                case 0x04: return "RAID bus controller";
                case 0x05:
                switch (prog_if) {
                    case 0x20: return "ADMA single stepping";
                    case 0x30: return "ADMA continuous operation";
                    default: return "ATA controller";
                }
                case 0x06:
                switch (prog_if) {
                    case 0x00: return "Vendor specific";
                    case 0x01: return "AHCI 1.0";
                    case 0x02: return "Serial Storage Bus";
                    default: return "SATA controller";
                }
                case 0x07:
                switch (prog_if) {
                    case 0x01: return "Serial Storage Bus";
                    default: return "Serial Attached SCSI controller";
                }
                case 0x08:
                switch (prog_if) {
                    case 0x01: return "NVMHCI";
                    case 0x02: return "NVM Express";
                    default: return "Non-Volatile memory controller";
                }
                case 0x80: return "Mass storage controller";
                default: return "Mass storage controller";
            }
        case 0x02:
            switch (subclass) {
                case 0x00: return "Ethernet controller";
                case 0x01: return "Token ring network controller";
                case 0x02: return "FDDI network controller";
                case 0x03: return "ATM network controller";
                case 0x04: return "ISDN controller";
                case 0x05: return "WorldFip controller";
                case 0x06: return "PICMG controller";
                case 0x07: return "Infiniband controller";
                case 0x08: return "Fabric controller";
                case 0x80: return "Network controller";
                default: return "Network controller";
            }
        case 0x03:
            switch (subclass) {
                case 0x00:
                switch (prog_if) {
                    case 0x00: return "VGA controller";
                    case 0x01: return "8514 controller";
                    default: return "VGA compatible controller";
                }
                case 0x01: return "XGA compatible controller";
                case 0x02: return "3D controller";
                case 0x80: return "Display controller";
                default: return "Display controller";
            }
        case 0x04:
            switch (subclass) {
                case 0x00: return "Multimedia video controller";
                case 0x01: return "Multimedia audio controller";
                case 0x02: return "Computer telephony device";
                case 0x03: return "Audio device";
                case 0x80: return "Multimedia controller";
                default: return "Multimedia controller";
            }
        case 0x05:
            switch (subclass) {
                case 0x00: return "RAM memory";
                case 0x01: return "FLASH memory";
                case 0x80: return "Memory controller";
                default: return "Memory controller";
            }
        case 0x06:
            switch (subclass) {
                case 0x00: return "Host bridge";
                case 0x01: return "ISA bridge";
                case 0x02: return "EISA bridge";
                case 0x03: return "MicroChannel bridge";
                case 0x04:
                switch (prog_if) {
                    case 0x00: return "Normal decode";
                    case 0x01: return "Subtractive decode";
                    default: return "PCI bridge";
                }
                case 0x05: return "PCMCIA bridge";
                case 0x06: return "NuBus bridge";
                case 0x07: return "CardBus bridge";
                case 0x08:
                switch (prog_if) {
                    case 0x00: return "Transparent mode";
                    case 0x01: return "Endpoint mode";
                    default: return "RACEway bridge";
                }
                case 0x09:
                switch (prog_if) {
                    case 0x40: return "Primary bus towards host CPU";
                    case 0x80: return "Secondary bus towards host CPU";
                    default: return "Semi-transparent PCI-to-PCI bridge";
                }
                case 0x0a: return "InfiniBand to PCI host bridge";
                case 0x80: return "Bridge";
                default: return "Bridge";
            }
        case 0x07:
            switch (subclass) {
                case 0x00:
                switch (prog_if) {
                    case 0x00: return "8250";
                    case 0x01: return "16450";
                    case 0x02: return "16550";
                    case 0x03: return "16650";
                    case 0x04: return "16750";
                    case 0x05: return "16850";
                    case 0x06: return "16950";
                    default: return "Serial controller";
                }
                case 0x01:
                switch (prog_if) {
                    case 0x00: return "SPP";
                    case 0x01: return "BiDir";
                    case 0x02: return "ECP";
                    case 0x03: return "IEEE1284";
                    case 0xfe: return "IEEE1284 Target";
                    default: return "Parallel controller";
                }
                case 0x02: return "Multiport serial controller";
                case 0x03:
                switch (prog_if) {
                    case 0x00: return "Generic";
                    case 0x01: return "Hayes/16450";
                    case 0x02: return "Hayes/16550";
                    case 0x03: return "Hayes/16650";
                    case 0x04: return "Hayes/16750";
                    default: return "Modem";
                }
                case 0x04: return "GPIB controller";
                case 0x05: return "Smard Card controller";
                case 0x80: return "Communication controller";
                default: return "Communication controller";
            }
        case 0x08:
            switch (subclass) {
                case 0x00:
                switch (prog_if) {
                    case 0x00: return "8259";
                    case 0x01: return "ISA PIC";
                    case 0x02: return "EISA PIC";
                    case 0x10: return "IO-APIC";
                    case 0x20: return "IO(X)-APIC";
                    default: return "PIC";
                }
                case 0x01:
                switch (prog_if) {
                    case 0x00: return "8237";
                    case 0x01: return "ISA DMA";
                    case 0x02: return "EISA DMA";
                    default: return "DMA controller";
                }
                case 0x02:
                switch (prog_if) {
                    case 0x00: return "8254";
                    case 0x01: return "ISA Timer";
                    case 0x02: return "EISA Timers";
                    case 0x03: return "HPET";
                    default: return "Timer";
                }
                case 0x03:
                switch (prog_if) {
                    case 0x00: return "Generic";
                    case 0x01: return "ISA RTC";
                    default: return "RTC";
                }
                case 0x04: return "PCI Hot-plug controller";
                case 0x05: return "SD Host controller";
                case 0x06: return "IOMMU";
                case 0x80: return "System peripheral";
                default: return "Generic system peripheral";
            }
        case 0x09:
            switch (subclass) {
                case 0x00: return "Keyboard controller";
                case 0x01: return "Digitizer Pen";
                case 0x02: return "Mouse controller";
                case 0x03: return "Scanner controller";
                case 0x04:
                switch (prog_if) {
                    case 0x00: return "Generic";
                    case 0x10: return "Extended";
                    default: return "Gameport controller";
                }
                case 0x80: return "Input device controller";
                default: return "Input device controller";
            }
        case 0x0a:
            switch (subclass) {
                case 0x00: return "Generic Docking Station";
                case 0x80: return "Docking Station";
                default: return "Docking station";
            }
        case 0x0b:
            switch (subclass) {
                case 0x00: return "386";
                case 0x01: return "486";
                case 0x02: return "Pentium";
                case 0x10: return "Alpha";
                case 0x20: return "Power PC";
                case 0x30: return "MIPS";
                case 0x40: return "Co-processor";
                default: return "Processor";
            }
        case 0x0c:
            switch (subclass) {
                case 0x00:
                switch (prog_if) {
                    case 0x00: return "Generic";
                    case 0x10: return "OHCI";
                    default: return "FireWire (IEEE 1394)";
                }
                case 0x01: return "ACCESS Bus";
                case 0x02: return "SSA";
                case 0x03:
                switch (prog_if) {
                    case 0x00: return "UHCI";
                    case 0x10: return "OHCI";
                    case 0x20: return "EHCI";
                    case 0x30: return "XHCI";
                    case 0x80: return "Unspecified";
                    case 0xfe: return "USB Device";
                    default: return "USB controller";
                }
                case 0x04: return "Fibre Channel";
                case 0x05: return "SMBus";
                case 0x06: return "InfiniBand";
                case 0x07: return "IPMI SMIC interface";
                case 0x08: return "SERCOS interface";
                case 0x09: return "CANBUS";
                default: return "Serial bus controller";
            }
        case 0x0d:
            switch (subclass) {
                case 0x00: return "IRDA controller";
                case 0x01: return "Consumer IR controller";
                case 0x10: return "RF controller";
                case 0x11: return "Bluetooth";
                case 0x12: return "Broadband";
                case 0x20: return "802.1a controller";
                case 0x21: return "802.1b controller";
                case 0x80: return "Wireless controller";
                default: return "Wireless controller";
            }
        case 0x0e:
            switch (subclass) {
                case 0x00: return "I2O";
                default: return "Intelligent controller";
            }
        case 0x0f:
            switch (subclass) {
                case 0x01: return "Satellite TV controller";
                case 0x02: return "Satellite audio communication controller";
                case 0x03: return "Satellite voice communication controller";
                case 0x04: return "Satellite data communication controller";
                default: return "Satellite communications controller";
            }
        case 0x10:
            switch (subclass) {
                case 0x00: return "Network and computing encryption device";
                case 0x10: return "Entertainment encryption device";
                case 0x80: return "Encryption controller";
                default: return "Encryption controller";
            }
        case 0x11:
            switch (subclass) {
                case 0x00: return "DPIO module";
                case 0x01: return "Performance counters";
                case 0x10: return "Communication synchronizer";
                case 0x20: return "Signal processing management";
                case 0x80: return "Signal processing controller";
                default: return "Signal processing controller";
            }
        case 0x12:
            switch (subclass) {
                case 0x00: return "Processing accelerators";
                default: return "Processing accelerators";
            }
        case 0x13: return "Non-Essential Instrumentation";
        case 0x40: return "Coprocessor";
        case 0xff: return "Unassigned class";
        default: return "Unassigned Class";
    }
}

