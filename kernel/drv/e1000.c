#include <netinet/in.h>
#include <ng/debug.h>
#include <ng/irq.h>
#include <ng/limine.h>
#include <ng/net.h>
#include <ng/pci.h>
#include <ng/pk.h>
#include <ng/pmm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 4096

#define TX_DESC_COUNT 80
#define RX_DESC_COUNT 80
#define TX_BUFFER_SIZE 2048
#define RX_BUFFER_SIZE 2048
#define TX_RING_PAGES \
	((TX_DESC_COUNT * sizeof(struct e1000_tx_desc) + PAGE_SIZE - 1) / PAGE_SIZE)
#define RX_RING_PAGES \
	((RX_DESC_COUNT * sizeof(struct e1000_rx_desc) + PAGE_SIZE - 1) / PAGE_SIZE)
#define TX_BUFFER_PAGES \
	((TX_DESC_COUNT * TX_BUFFER_SIZE + PAGE_SIZE - 1) / PAGE_SIZE)
#define RX_BUFFER_PAGES \
	((TX_DESC_COUNT * RX_BUFFER_SIZE + PAGE_SIZE - 1) / PAGE_SIZE)

#define r32(reg) (*(volatile uint32_t *)(e->mmio_base + reg))
#define w32(reg, val) (*(volatile uint32_t *)(e->mmio_base + reg) = val)

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

static_assert(sizeof(struct e1000_tx_desc) == 16);
static_assert(sizeof(struct e1000_rx_desc) == 16);
static_assert(TX_DESC_COUNT * sizeof(struct e1000_tx_desc) % 128 == 0);
static_assert(RX_DESC_COUNT * sizeof(struct e1000_rx_desc) % 128 == 0);

struct e1000 {
	struct net_if nif;

	pci_address_t addr;

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

	int tx;
	int rx;
};

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
	w32(IMS, INT_ALL);
	w32(IMC, INT_ALL);

	w32(CTRL, CTRL_RST);
	while (r32(CTRL) & CTRL_RST)
		asm volatile("pause");

	for (int i = 0; i < 100'000; i++)
		asm volatile("pause");

	w32(IMC, INT_ALL);

	__atomic_thread_fence(__ATOMIC_SEQ_CST);
}

static uint16_t e1000_eeprom_read(struct e1000 *e, uint8_t addr) {
	w32(EERD, 1 | (addr << 8));
	while (!(r32(EERD) & 0x10))
		asm volatile("pause");
	return r32(EERD) >> 16;
}

static void e1000_eeprom_read_mac(struct e1000 *e) {
	for (int i = 0; i < 3; i++) {
		uint16_t data = e1000_eeprom_read(e, i);
		e->mac.addr[i * 2] = data & 0xFF;
		e->mac.addr[i * 2 + 1] = data >> 8;
	}
}

static void e1000_read_irq(struct e1000 *e) {
	int irq = pci_read8(e->addr, 0x3C);
	printf("e1000: irq %d\n", irq);
	e->irq = irq;
}

static void e1000_set_rx_mac(struct e1000 *e) {
	uint32_t ral = e->mac.addr[0] | e->mac.addr[1] << 8 | e->mac.addr[2] << 16
		| e->mac.addr[3] << 24;
	uint32_t rah = e->mac.addr[4] | e->mac.addr[5] << 8 | 1u << 31;
	w32(RAL, ral);
	w32(RAH, rah);
}

static void e1000_disable_interrupts(struct e1000 *e, uint32_t mask) {
	w32(IMC, mask);
}

static void e1000_enable_interrupts(struct e1000 *e, uint32_t mask) {
	w32(IMS, mask);
}

// static void e1000_trigger_interrupt(struct e1000 *e, uint32_t mask) {
// 	w32(ICS, mask);
// }

static uint32_t e1000_read_interrupt_causes(struct e1000 *e) {
	return r32(ICR);
}

static void e1000_acknowledge_interrupts(struct e1000 *e, uint32_t mask) {
	w32(ICR, mask);
}

static void e1000_set_link_state(struct e1000 *e) {
	uint32_t ctrl = r32(CTRL);
	ctrl |= CTRL_SLU | CTRL_SPEED_1000;
	ctrl &= ~CTRL_PHY_RST;
	w32(CTRL, ctrl);
}

static void e1000_enable_rx(struct e1000 *e) {
	w32(RDBAL, e->rx_ring_phy);
	w32(RDBAH, 0);
	w32(RDLEN, RX_DESC_COUNT * sizeof(struct e1000_rx_desc));
	w32(RDH, 0);
	w32(RDT, RX_DESC_COUNT - 1);

	w32(RDTR, 0);
	w32(RADV, 0);
	w32(RSRPD, 0);

	w32(ITR, 651); // 1'000'000 / 10'000 * 4);
	w32(FCRTL, 0);
	w32(FCRTH, 0);

	w32(RCTL, (1 << 1) | (1 << 3) | (1 << 4) | (1 << 15));
}

static void e1000_enable_tx(struct e1000 *e) {
	w32(TDBAL, e->tx_ring_phy);
	w32(TDBAH, 0);
	w32(TDLEN, TX_DESC_COUNT * sizeof(struct e1000_tx_desc));
	w32(TDH, 0);
	w32(TDT, 0);

	w32(TIPG, 10 | (8 << 10) | (6 << 20));

	w32(TCTL, (1 << 1) | (1 << 3));
}

static void e1000_log_link_status(struct e1000 *e) {
	uint32_t status = r32(STATUS);
	if (status & 2) {
		printf("e1000: link up\n");
	} else {
		printf("e1000: link down\n");
	}
}

static void e1000_send(struct e1000 *e, void *data, size_t len) {
	int i = 0;
	uint32_t tail = e->tx;

	struct e1000_tx_desc *desc = &e->tx_descs[tail];
	memcpy(e->tx_buffer + tail * TX_BUFFER_SIZE, data, len);
	desc->length = len;
	desc->cmd = CMD_EOP | CMD_IFCS | CMD_RS | CMD_RPS;
	desc->status = 0;
	tail = (tail + 1) % TX_DESC_COUNT;

	printf("e1000: sending %zu bytes, tail is now %u\n", len, tail);

	__atomic_thread_fence(__ATOMIC_RELEASE);

	e->tx = tail;
	w32(TDT, tail);
}

void net_ingress(struct pk *pk);

static void e1000_receive(struct e1000 *e) {
	uint32_t head = r32(RDH);
	uint32_t tail = e->rx;

	while (head != tail) {
		printf("head: %u, tail: %u\n", head, tail);

		struct e1000_rx_desc *desc = &e->rx_descs[tail];
		if (!(desc->status & 1))
			break;

		void *data = e->rx_buffer + tail * RX_BUFFER_SIZE;
		struct pk *pk = pk_alloc();
		pk->len = desc->length;
		memcpy(pk->data, data, desc->length);
		net_ingress(pk);

		desc->status = 0;

		__atomic_thread_fence(__ATOMIC_RELEASE);

		w32(RDT, tail);

		tail = (tail + 1) % RX_DESC_COUNT;
		e->rx = tail;
	}
}

static void e1000_init(struct e1000 *e) {
	pci_enable_bus_mastering(e->addr);

	for (int i = 0; i < 2; i++) {
		uint32_t bar = pci_get_bar(e->addr, i);
		if (bar & 1) {
			e->io_base = bar & ~1;
		} else {
			e->mmio_base = bar | limine_hhdm();
		}
		printf("e1000 bar%d: %x\n", i, bar);
	}

	e1000_reset(e);
	e1000_eeprom_read_mac(e);
	printf("e1000 mac: %02x:%02x:%02x:%02x:%02x:%02x\n", e->mac.addr[0],
		e->mac.addr[1], e->mac.addr[2], e->mac.addr[3], e->mac.addr[4],
		e->mac.addr[5]);
	e1000_set_rx_mac(e);
	e1000_read_irq(e);

	// 13.4.10, "this register should be programmed with 88_08h"
	w32(FCT, 0x8808);
	// 13.4.11, "this register should be programmed with 8100h"
	w32(VET, 0x8100);

	e->tx_ring_phy = pm_alloc_contiguous(TX_RING_PAGES);
	e->tx_descs = (struct e1000_tx_desc *)(e->tx_ring_phy | limine_hhdm());
	e->rx_ring_phy = pm_alloc_contiguous(RX_RING_PAGES);
	e->rx_descs = (struct e1000_rx_desc *)(e->rx_ring_phy | limine_hhdm());
	e->tx_buffer_phy = pm_alloc_contiguous(TX_BUFFER_PAGES);
	e->tx_buffer = (void *)(e->tx_buffer_phy | limine_hhdm());
	e->rx_buffer_phy = pm_alloc_contiguous(RX_BUFFER_PAGES);
	e->rx_buffer = (void *)(e->rx_buffer_phy | limine_hhdm());

	memset(e->tx_descs, 0, TX_DESC_COUNT * sizeof(struct e1000_tx_desc));
	memset(e->rx_descs, 0, RX_DESC_COUNT * sizeof(struct e1000_rx_desc));
	memset(e->tx_buffer, 0, TX_DESC_COUNT * TX_BUFFER_SIZE);
	memset(e->rx_buffer, 0, RX_DESC_COUNT * RX_BUFFER_SIZE);

	// Initialize rx ring
	for (int i = 0; i < RX_DESC_COUNT; i++) {
		e->rx_descs[i].addr = e->rx_buffer_phy + i * RX_BUFFER_SIZE;
		e->rx_descs[i].status = 0;
	}

	// Initialize tx ring
	for (int i = 0; i < TX_DESC_COUNT; i++) {
		e->tx_descs[i].addr = e->tx_buffer_phy + i * TX_BUFFER_SIZE;
		e->tx_descs[i].status = 0;
	}

	e1000_disable_interrupts(e, INT_ALL);

	e1000_set_link_state(e);
	e1000_enable_rx(e);
	e1000_enable_tx(e);

	e1000_enable_interrupts(e, INT_LSC);
	e1000_enable_interrupts(e, INT_RXO | INT_RXTO);
	// e1000_enable_interrupts(e, INT_TXDW | INT_TXQE);
	// e1000_enable_interrupts(e, INT_ALL);
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

void e1000_test(pci_address_t addr) {
	struct e1000 *e = calloc(1, sizeof(struct e1000));
	e->addr = addr;

	e1000_init(e);
	e1000_log_link_status(e);

	uint32_t interrupt_state = r32(IMS);
	printf("e1000: interrupt state %x\n", interrupt_state);

	irq_install(e->irq, e1000_handle_interrupt, e);

	// unsigned char ethernet_frame[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	// 0x52, 	0x54, 0x00, 0x12, 0x34, 0x56, 0x08, 0x06, 0x00, 0x01, 0x08,
	// 0x00, 0x06, 	0x04, 0x00, 0x01, 0x52, 0x54, 0x00, 0x12, 0x34, 0x56, 0x0a,
	// 0x00, 0x02, 	0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x02,
	// 0x02 }; printf("sending frame:\n");

	// net_debug(0, ethernet_frame, sizeof(ethernet_frame));

	// e1000_send(e, ethernet_frame, sizeof(ethernet_frame));
}
