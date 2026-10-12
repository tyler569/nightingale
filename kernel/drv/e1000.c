#include <ng/irq.h>
#include <ng/limine.h>
#include <ng/net.h>
#include <ng/pci.h>
#include <ng/pk.h>
#include <ng/pmm.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void net_debug(int, void *, size_t);

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
	struct net_if nif;

	pci_address_t addr;

	virt_addr_t mmio_base;
	uint16_t io_base;
	int irq;

	phys_addr_t tx_ring_phy;
	struct e1000_tx_desc *tx_descs;
	phys_addr_t rx_ring_phy;
	struct e1000_rx_desc *rx_descs;

	phys_addr_t tx_buffer_phy;
	void *tx_buffer;
	phys_addr_t rx_buffer_phy;
	void *rx_buffer;

	int tx;
	int rx;
};

static constexpr size_t PAGE_SIZE = 4096;
static constexpr size_t TX_DESC_COUNT = 80;
static constexpr size_t RX_DESC_COUNT = 80;
static constexpr size_t TX_BUFFER_SIZE = 2048;
static constexpr size_t RX_BUFFER_SIZE = 2048;
static constexpr size_t TX_RING_PAGES =
	((TX_DESC_COUNT * sizeof(struct e1000_tx_desc) + PAGE_SIZE - 1) / PAGE_SIZE);
static constexpr size_t RX_RING_PAGES =
	((RX_DESC_COUNT * sizeof(struct e1000_rx_desc) + PAGE_SIZE - 1) / PAGE_SIZE);
static constexpr size_t TX_BUFFER_PAGES =
	((TX_DESC_COUNT * TX_BUFFER_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
static constexpr size_t RX_BUFFER_PAGES =
	((TX_DESC_COUNT * RX_BUFFER_SIZE + PAGE_SIZE - 1) / PAGE_SIZE);
static_assert(sizeof(struct e1000_tx_desc) == 16);
static_assert(sizeof(struct e1000_rx_desc) == 16);
static_assert(TX_DESC_COUNT * sizeof(struct e1000_tx_desc) % 128 == 0);
static_assert(RX_DESC_COUNT * sizeof(struct e1000_rx_desc) % 128 == 0);

static inline uint32_t read_reg(struct e1000 *e, size_t reg) {
	return *(volatile uint32_t *)(e->mmio_base + reg);
}

static inline void write_reg(struct e1000 *e, size_t reg, uint32_t val) {
	*(volatile uint32_t *)(e->mmio_base + reg) = val;
}

enum e1000_reg {
	CTRL = 0x00000,
	STATUS = 0x00008,
	EECD = 0x00010,
	EERD = 0x00014,
	FCT = 0x00030,
	VET = 0x00038,
	ICR = 0x000C0,
	ITR = 0x000C4,
	ICS = 0x000C8,
	IMS = 0x000D0,
	IMC = 0x000D8,
	RCTL = 0x00100,
	TCTL = 0x00400,
	TIPG = 0x00410,

	FCRTL = 0x02160,
	FCRTH = 0x02168,

	RDBAL = 0x02800,
	RDBAH = 0x02804,
	RDLEN = 0x02808,
	RDH = 0x02810,
	RDT = 0x02818,
	RDTR = 0x02820,
	RADV = 0x0282C,
	RSRPD = 0x02C00,

	TDBAL = 0x03800,
	TDBAH = 0x03804,
	TDLEN = 0x03808,
	TDH = 0x03810,
	TDT = 0x03818,

	RAL = 0x05400,
	RAH = 0x05404,
};

enum e1000_ctrl {
	CTRL_SLU = 1 << 6,
	CTRL_SPEED_1000 = 1 << 9,
	CTRL_RST = 1 << 26,
	CTRL_PHY_RST = 1 << 31,
};

enum e1000_interrupt {
	INT_TXDW = 1 << 0, // Transmit descriptor written back
	INT_TXQE = 1 << 1, // Transmit queue empty
	INT_LSC = 1 << 2, // Link status change
	INT_RXSEQ = 1 << 3, // Receive sequence error
	INT_RXDMTO = 1 << 4, // Receive descriptor minimum threshold
	INT_RXO = 1 << 6, // Receive overrun
	INT_RXTO = 1 << 7, // Receive timer interrupt
	INT_MDAC = 1 << 9, // MDIO access complete
	INT_RXCFG = 1 << 10, // Receive configuration change
	INT_PHY = 1 << 12, // PHY interrupt
	INT_GPI = 1 << 13, // General purpose interrupt
	INT_TXD_LOW = 1 << 15, // Transmit descriptor low
	INT_SRPD = 1 << 16, // Small receive packet detected

	INT_ALL = 0x0000FFFF,
};

enum e1000_cmd {
	CMD_EOP = 1 << 0, // End of packet
	CMD_IFCS = 1 << 1, // Insert FCS
	CMD_IC = 1 << 2, // Insert checksum
	CMD_RS = 1 << 3, // Report status
	CMD_RPS = 1 << 4, // Report packet sent
	CMD_DEXT = 1 << 5, // Descriptor extension
	CMD_VLE = 1 << 6, // VLAN packet enable
	CMD_IDE = 1 << 7, // Interrupt delay enable
};

static void e1000_reset(struct e1000 *e) {
	write_reg(e, IMS, INT_ALL);
	write_reg(e, IMC, INT_ALL);

	write_reg(e, CTRL, CTRL_RST);
	while (read_reg(e, CTRL) & CTRL_RST)
		asm volatile("pause");

	for (int i = 0; i < 100'000; i++)
		asm volatile("pause");

	write_reg(e, IMC, INT_ALL);

	__atomic_thread_fence(memory_order_seq_cst);
}

static uint16_t e1000_eeprom_read(struct e1000 *e, uint8_t addr) {
	write_reg(e, EERD, 1 | (addr << 8));
	while (!(read_reg(e, EERD) & 0x10))
		asm volatile("pause");
	return read_reg(e, EERD) >> 16;
}

static void e1000_eeprom_read_mac(struct e1000 *e) {
	for (int i = 0; i < 3; i++) {
		uint16_t data = e1000_eeprom_read(e, i);
		e->nif.mac.addr[i * 2] = data & 0xFF;
		e->nif.mac.addr[i * 2 + 1] = data >> 8;
	}
}

static void e1000_read_irq(struct e1000 *e) {
	int irq = pci_read8(e->addr, 0x3C);
	printf("e1000: irq %d\n", irq);
	e->irq = irq;
}

static void e1000_set_rx_mac(struct e1000 *e) {
	uint32_t ral = e->nif.mac.addr[0] | e->nif.mac.addr[1] << 8
		| e->nif.mac.addr[2] << 16 | e->nif.mac.addr[3] << 24;
	uint32_t rah = e->nif.mac.addr[4] | e->nif.mac.addr[5] << 8 | 1u << 31;
	write_reg(e, RAL, ral);
	write_reg(e, RAH, rah);
}

static void e1000_disable_interrupts(struct e1000 *e, uint32_t mask) {
	write_reg(e, IMC, mask);
}

static void e1000_enable_interrupts(struct e1000 *e, uint32_t mask) {
	write_reg(e, IMS, mask);
}

// static void e1000_trigger_interrupt(struct e1000 *e, uint32_t mask) {
// 	write_reg(e, ICS, mask);
// }

static uint32_t e1000_read_interrupt_causes(struct e1000 *e) {
	return read_reg(e, ICR);
}

static void e1000_acknowledge_interrupts(struct e1000 *e, uint32_t mask) {
	write_reg(e, ICR, mask);
}

static void e1000_set_link_state(struct e1000 *e) {
	uint32_t ctrl = read_reg(e, CTRL);
	ctrl |= CTRL_SLU | CTRL_SPEED_1000;
	ctrl &= ~CTRL_PHY_RST;
	write_reg(e, CTRL, ctrl);
}

static void e1000_enable_rx(struct e1000 *e) {
	write_reg(e, RDBAL, e->rx_ring_phy);
	write_reg(e, RDBAH, 0);
	write_reg(e, RDLEN, RX_DESC_COUNT * sizeof(struct e1000_rx_desc));
	write_reg(e, RDH, 0);
	write_reg(e, RDT, RX_DESC_COUNT - 1);

	write_reg(e, RDTR, 0);
	write_reg(e, RADV, 0);
	write_reg(e, RSRPD, 0);

	write_reg(e, ITR, 651); // 1'000'000 / 10'000 * 4);
	write_reg(e, FCRTL, 0);
	write_reg(e, FCRTH, 0);

	write_reg(e, RCTL, (1 << 1) | (1 << 3) | (1 << 4) | (1 << 15));
}

static void e1000_enable_tx(struct e1000 *e) {
	write_reg(e, TDBAL, e->tx_ring_phy);
	write_reg(e, TDBAH, 0);
	write_reg(e, TDLEN, TX_DESC_COUNT * sizeof(struct e1000_tx_desc));
	write_reg(e, TDH, 0);
	write_reg(e, TDT, 0);

	write_reg(e, TIPG, 10 | (8 << 10) | (6 << 20));

	write_reg(e, TCTL, (1 << 1) | (1 << 3));
}

static void e1000_log_link_status(struct e1000 *e) {
	uint32_t status = read_reg(e, STATUS);
	if (status & 2) {
		printf("e1000: link up\n");
	} else {
		printf("e1000: link down\n");
	}
}

static void e1000_send(struct e1000 *e, void *data, size_t len) {
	int tail = e->tx;

	struct e1000_tx_desc *desc = &e->tx_descs[tail];
	memcpy(e->tx_buffer + tail * TX_BUFFER_SIZE, data, len);
	desc->length = len;
	desc->cmd = CMD_EOP | CMD_IFCS | CMD_RS | CMD_RPS;
	desc->status = 0;
	tail = (tail + 1) % TX_DESC_COUNT;

	printf("e1000: sending %zu bytes, tail is now %u\n", len, tail);

	__atomic_thread_fence(memory_order_release);

	e->tx = tail;
	write_reg(e, TDT, tail);
}

static void e1000_receive(struct e1000 *e) {
	int head = read_reg(e, RDH);
	int tail = e->rx;

	while (head != tail) {
		printf("head: %u, tail: %u\n", head, tail);

		struct e1000_rx_desc *desc = &e->rx_descs[tail];
		if (!(desc->status & 1))
			break;

		void *data = e->rx_buffer + tail * RX_BUFFER_SIZE;

		// TODO: don't call ingress in the interrupt,
		// this has to go to a queue.
		struct pk *pk = pk_alloc();
		pk->len = desc->length;
		pk->origin_if = &e->nif;
		memcpy(pk->data, data, desc->length);
		net_ingress(pk);

		desc->status = 0;
		desc->length = RX_BUFFER_SIZE;

		__atomic_thread_fence(memory_order_release);

		write_reg(e, RDT, tail);

		tail = (tail + 1) % RX_DESC_COUNT;
		e->rx = tail;
		head = read_reg(e, RDH);
	}
}

static void e1000_handle_interrupt(interrupt_frame *, void *data) {
	printf("e1000: interrupt %p\n", data);
	struct e1000 *e = data;
	uint32_t icr = e1000_read_interrupt_causes(e);

	printf("e1000: interrupt cause %#x\n", icr);

	if (icr & 0x04) {
		e1000_log_link_status(e);
	} else if (icr & 0xD0) {
		e1000_receive(e);
	}

	e1000_acknowledge_interrupts(e, icr);
}

static void e1000_send_pk(struct net_if *nif, struct pk *pk) {
	struct e1000 *e = container_of(struct e1000, nif, nif);
	e1000_send(e, pk->data, pk->len);
}

static struct net_if_vtbl net_e1000_vtbl = {
	.send = e1000_send_pk,
};

static void e1000_init(struct e1000 *e, pci_address_t addr) {
	e->addr = addr;
	e->nif.vtbl = &net_e1000_vtbl;

	pci_enable_bus_mastering(e->addr);

	for (int i = 0; i < 2; i++) {
		uint32_t bar = pci_get_bar(e->addr, i);
		if (bar & 1) {
			e->io_base = bar & ~1;
		} else {
			e->mmio_base = virtual_of(bar);
		}
		printf("e1000 bar%d: %x\n", i, bar);
	}

	e1000_reset(e);
	e1000_eeprom_read_mac(e);
	printf("e1000 mac: %02x:%02x:%02x:%02x:%02x:%02x\n", e->nif.mac.addr[0],
		e->nif.mac.addr[1], e->nif.mac.addr[2], e->nif.mac.addr[3],
		e->nif.mac.addr[4], e->nif.mac.addr[5]);
	e1000_set_rx_mac(e);
	e1000_read_irq(e);

	// 13.4.10, "this register should be programmed with 88_08h"
	write_reg(e, FCT, 0x8808);
	// 13.4.11, "this register should be programmed with 8100h"
	write_reg(e, VET, 0x8100);

	e->tx_ring_phy = pm_alloc_contiguous(TX_RING_PAGES);
	e->tx_descs = (struct e1000_tx_desc *)virtual_of(e->tx_ring_phy);
	e->rx_ring_phy = pm_alloc_contiguous(RX_RING_PAGES);
	e->rx_descs = (struct e1000_rx_desc *)virtual_of(e->rx_ring_phy);
	e->tx_buffer_phy = pm_alloc_contiguous(TX_BUFFER_PAGES);
	e->tx_buffer = (void *)virtual_of(e->tx_buffer_phy);
	e->rx_buffer_phy = pm_alloc_contiguous(RX_BUFFER_PAGES);
	e->rx_buffer = (void *)virtual_of(e->rx_buffer_phy);

	memset(e->tx_descs, 0, TX_DESC_COUNT * sizeof(struct e1000_tx_desc));
	memset(e->rx_descs, 0, RX_DESC_COUNT * sizeof(struct e1000_rx_desc));
	memset(e->tx_buffer, 0, TX_DESC_COUNT * TX_BUFFER_SIZE);
	memset(e->rx_buffer, 0, RX_DESC_COUNT * RX_BUFFER_SIZE);

	// Initialize rx ring
	for (size_t i = 0; i < RX_DESC_COUNT; i++) {
		e->rx_descs[i].addr = e->rx_buffer_phy + i * RX_BUFFER_SIZE;
		e->rx_descs[i].status = 0;
	}

	// Initialize tx ring
	for (size_t i = 0; i < TX_DESC_COUNT; i++) {
		e->tx_descs[i].addr = e->tx_buffer_phy + i * TX_BUFFER_SIZE;
		e->tx_descs[i].status = 0;
	}

	e1000_disable_interrupts(e, INT_ALL);

	e1000_set_link_state(e);
	e1000_enable_rx(e);
	e1000_enable_tx(e);
	e1000_log_link_status(e);

	e->nif.ip.s_addr = 0x0f02000a; // 10.0.2.15
	e->nif.netmask.s_addr = 0x00ffffff; // 255.255.255.0
	e->nif.gateway.s_addr = 0x0202000a; // 10.0.2.2

	irq_install(e->irq, e1000_handle_interrupt, e);

	e1000_enable_interrupts(e, INT_LSC);
	e1000_enable_interrupts(e, INT_RXO | INT_RXTO);
	// e1000_enable_interrupts(e, INT_TXDW | INT_TXQE);
	// e1000_enable_interrupts(e, INT_ALL);

	if (!default_if)
		default_if = &e->nif;
}

static const struct pci_device_id e1000_ids[] = {
	{ .vendor = 0x8086, .device = 0x100e },
	{},
};

static int e1000_probe(pci_address_t addr, const struct pci_device_id *) {
	struct e1000 *e = zmalloc(sizeof(struct e1000));
	e1000_init(e, addr);
	return 0;
}

static struct pci_driver e1000_driver = {
	.name = "e1000",
	.ids = e1000_ids,
	.probe = e1000_probe,
};
define_pci_driver(e1000_driver);
