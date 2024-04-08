#include <netinet/in.h>
#include <ng/limine.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <stdio.h>

#define TX_RING_SIZE 256
#define RX_RING_SIZE 256
#define TX_BUFFER_SIZE 2048
#define RX_BUFFER_SIZE 2048

struct e1000_tx_desc {
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
};

struct e1000_rx_desc {
	uint64_t addr;
	uint16_t length;
	uint16_t csum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
};

struct e1000 {
	virt_addr_t mmio_base;
	uint16_t io_base;
	int irq;

	struct eth_addr mac;

	phys_addr_t tx_ring_phy;
	struct e1000_tx_desc *tx_descs;
	phys_addr_t rx_ring_phy;
	struct e1000_rx_desc *rx_descs;

	phys_addr_t tx_buffer_phy;
	void *tx_buffer;
	phys_addr_t rx_buffer_phy;
	void *rx_buffer;
};

enum e1000_reg {
	CTRL = 0x0000,
	STATUS = 0x0008,
	EECD = 0x1000,
	EERD = 0x1004,
	CTRL_EXT = 0x0018,
	ICR = 0x00C0,
	ITR = 0x00C4,
	RCTL = 0x0100,
	TCTL = 0x0400,
	TIPG = 0x0410,
	RDBAL = 0x2800,
	RDBAH = 0x2804,
	RDLEN = 0x2808,
	RDH = 0x2810,
	RDT = 0x2818,
	TDBAL = 0x3800,
	TDBAH = 0x3804,
	TDLEN = 0x3808,
	TDH = 0x3810,
	TDT = 0x3818,
};

enum e1000_ctrl {
	CTRL_RST = 0x04000000,
	CTRL_ASDE = 0x00000020,
	CTRL_SLU = 0x00000040,
	CTRL_FD = 0x00000008,
	CTRL_EN = 0x00000002,
};

static void reset(struct e1000 *e) {
	uint32_t ctrl = pci_mmio_read32(e->mmio_base, CTRL);
	ctrl |= CTRL_RST;
	pci_mmio_write32(e->mmio_base, CTRL, ctrl);
}

void e1000_init(struct e1000 *e, pci_address_t addr) {
	size_t tx_ring_pages = TX_RING_SIZE * sizeof(struct e1000_tx_desc) / 4096;
	size_t rx_ring_pages = RX_RING_SIZE * sizeof(struct e1000_rx_desc) / 4096;

	e->tx_ring_phy = pm_alloc_contiguous(tx_ring_pages);
	e->tx_descs = (struct e1000_tx_desc *)(e->tx_ring_phy | limine_hhdm());
	e->rx_ring_phy = pm_alloc_contiguous(rx_ring_pages);
	e->rx_descs = (struct e1000_rx_desc *)(e->rx_ring_phy | limine_hhdm());

	e->tx_buffer_phy
		= pm_alloc_contiguous(TX_RING_SIZE * TX_BUFFER_SIZE / 4096);
	e->tx_buffer = (void *)(e->tx_buffer_phy | limine_hhdm());
	e->rx_buffer_phy
		= pm_alloc_contiguous(RX_RING_SIZE * RX_BUFFER_SIZE / 4096);
	e->rx_buffer = (void *)(e->rx_buffer_phy | limine_hhdm());

	for (int i = 0; i < TX_RING_SIZE; i++) {
		e->tx_descs[i].addr = e->tx_buffer_phy + i * TX_BUFFER_SIZE;
		e->tx_descs[i].length = 0;
		e->tx_descs[i].cso = 0;
		e->tx_descs[i].cmd = 0;
		e->tx_descs[i].status = 0;
		e->tx_descs[i].css = 0;
		e->tx_descs[i].special = 0;
	}

	for (int i = 0; i < RX_RING_SIZE; i++) {
		e->rx_descs[i].addr = e->rx_buffer_phy + i * RX_BUFFER_SIZE;
		e->rx_descs[i].length = 0;
		e->rx_descs[i].csum = 0;
		e->rx_descs[i].status = 0;
		e->rx_descs[i].errors = 0;
		e->rx_descs[i].special = 0;
	}

	for (int i = 0; i < 6; i++) {
		e->mac.addr[i] = pci_mmio_read8(e->mmio_base, i);
	}

	pci_enable_bus_mastering(addr);
	reset(e);
}