#include <ng/drv/rtl8139.h>
#include <stdio.h>

rtl8139::rtl8139(pci_address pci_address) noexcept
    : pci_device(pci_address)
{
    if (vendor_id() != 0x10ec || device_id() != 0x8139) {
        printf("rtl8139: not found\n");
        return;
    }
}

void rtl8139::init() noexcept { printf("rtl8139: init\n"); }

void rtl8139::send_packet(const void *data, size_t len)
{
    printf("rtl8139: send_packet\n");
}

void rtl8139::recv_packet(void *data, size_t len)
{
    printf("rtl8139: recv_packet\n");
}