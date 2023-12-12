#include <ng/drv/nic_rtl8139.h>
#include <stdio.h>

nic_rtl8139::nic_rtl8139(pci_address pci_address)
    : m_pci_address(pci_address)
{
    printf("nic_rtl8139: constructor\n");
    printf("  bar0: %#x\n", pci_read32(m_pci_address, PCI_BAR0));
    printf("  bar1: %#x\n", pci_read32(m_pci_address, PCI_BAR1));
    printf("  bar2: %#x\n", pci_read32(m_pci_address, PCI_BAR2));
    printf("  bar3: %#x\n", pci_read32(m_pci_address, PCI_BAR3));
    printf("  bar4: %#x\n", pci_read32(m_pci_address, PCI_BAR4));
    printf("  bar5: %#x\n", pci_read32(m_pci_address, PCI_BAR5));
}