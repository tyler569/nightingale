#include <netinet/in.h>
#include <ng/common.h>
#include <ng/debug.h>
#include <ng/irq.h>
#include <ng/limine.h>
#include <ng/pci.h>
#include <ng/pmm.h>
#include <ng/vmm.h>
#include <stdio.h>
#include <string.h>

#define VERBOSE 1

#ifdef VERBOSE
#define verbose_printf(...) printf(__VA_ARGS__)
#else
#define verbose_printf(...)
#endif

struct rtl8139 {
	struct eth_addr mac;

	uint16_t io_base;
	virt_addr_t mmio_base;
	int irq;

	char tx_slot;

	phys_addr_t rx_buffer_phy;
	void *rx_buffer;
	size_t rx_index;
};

enum rtl8139_reg {
	MAC0 = 0x00,
	TSD0 = 0x10,
	TAD0 = 0x20,
	RBSTART = 0x30,
	CR = 0x37,
	CAPR = 0x38,
	IMR = 0x3C,
	ISR = 0x3E,
	TXCFG = 0x40,
	RXCFG = 0x44,
	CONFIG0 = 0x52,
	CONFIG1 = 0x53,
};

enum rtl8139_cmd {
	CR_RX_BUF_EMPTY = 0x01,
	CR_TX_ENABLE = 0x04,
	CR_RX_ENABLE = 0x08,
	CR_RESET = 0x10,
	TX_CONFIG_DMA_ALL = 0x700,
	TX_CONFIG_CRC = 0x1000,
	RX_CONFIG_ACCEPT_ALL = 0x0F,
	RX_CONFIG_WRAP = 0x80,
	RX_CONFIG_DMA_ALL = 0x700,
	RX_CONFIG_16K = 0x800,
	INTR_RX_OK = 0x01,
	INTR_RX_ERR = 0x02,
	INTR_TX_OK = 0x04,
	INTR_TX_ERR = 0x08,
	TX_OWN = 0x100,
	TX_OK = 0x400,
};

#define RX_BUFFER_PAGES 17
#define RX_BUFFER_SIZE (RX_BUFFER_PAGES * 4096)

static void debug_print(struct rtl8139 *r);
static void reset(struct rtl8139 *r);
static void set_rx_buffer(struct rtl8139 *r);
static void setup_txrx(struct rtl8139 *r);
static void enable_interrupts(struct rtl8139 *r, int);
static void enable_txrx(struct rtl8139 *r);
static void tx_write(struct rtl8139 *r, uint32_t data, size_t size);
static bool rx_empty(struct rtl8139 *r);

void rtl8139_init(struct rtl8139 *r, pci_address_t pci_address) {
	verbose_printf("rtl8139: init\n");

	uint32_t bar0 = pci_read32(pci_address, PCI_BAR0);
	uint32_t bar1 = pci_read32(pci_address, PCI_BAR1);
	if ((bar0 & 1) == 1) {
		r->mmio_base = bar0 & ~0xF;
	} else {
		printf("rtl8139: portio not supported\n");
	}
	if ((bar1 & 1) == 0) {
		r->mmio_base = (bar1 & ~0xF) | limine_hhdm();
	} else {
		printf("rtl8139: mmio not supported\n");
	}

	for (int i = 0; i < 6; i++) {
		r->mac.addr[i] = pci_mmio_read8(r->mmio_base, i);
	}

	pci_enable_bus_mastering(pci_address);
	reset(r);
	set_rx_buffer(r);
	setup_txrx(r);
	enable_interrupts(r, INTR_RX_OK | INTR_RX_ERR | INTR_TX_OK | INTR_TX_ERR);
	enable_txrx(r);
#ifdef VERBOSE
	debug_print(r);
#endif
}

static void debug_print(struct rtl8139 *r) {
	printf("rtl8139: io_base: %#x\n", r->io_base);
	printf("rtl8139: mmio_base: %#lx\n", r->mmio_base);

	printf("rtl8139: mac address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		r->mac.addr[0], r->mac.addr[1], r->mac.addr[2], r->mac.addr[3],
		r->mac.addr[4], r->mac.addr[5]);
}

static void reset(struct rtl8139 *r) {
	pci_mmio_write8(r->mmio_base, CONFIG0, 0);
	pci_mmio_write8(r->mmio_base, CR, CR_RESET);
	while (pci_mmio_read8(r->mmio_base, CR) & CR_RESET) {
		asm volatile("pause");
	}
}

static void set_rx_buffer(struct rtl8139 *r) {
	if (r->rx_buffer_phy == 0) {
		r->rx_buffer_phy = pm_alloc_contiguous(RX_BUFFER_PAGES);
		r->rx_buffer = (void *)(r->rx_buffer_phy | limine_hhdm());
		r->rx_index = 0;
	}

	if (r->rx_buffer_phy >= 1l << 32) {
		printf("rtl8139: rx buffer above 4GB\n");
	}

	pci_mmio_write32(r->mmio_base, RBSTART, r->rx_buffer_phy);
}

static void setup_txrx(struct rtl8139 *r) {
	pci_mmio_write32(r->mmio_base, TXCFG, TX_CONFIG_CRC | TX_CONFIG_DMA_ALL);
	pci_mmio_write32(r->mmio_base, RXCFG,
		RX_CONFIG_ACCEPT_ALL | RX_CONFIG_WRAP | RX_CONFIG_DMA_ALL
			| RX_CONFIG_16K);
}

static void enable_interrupts(struct rtl8139 *r, int mask) {
	pci_mmio_write16(r->mmio_base, IMR, mask);
}

static void enable_txrx(struct rtl8139 *r) {
	pci_mmio_write8(r->mmio_base, CR, CR_RX_ENABLE | CR_TX_ENABLE);
}

static uint16_t ack_interrupts(struct rtl8139 *r) {
	return pci_mmio_read16(r->mmio_base, ISR);
}

static uint32_t tx_status(struct rtl8139 *rtl8139) {
	return pci_mmio_read32(rtl8139->mmio_base, TSD0 + rtl8139->tx_slot * 4);
}

bool rx_empty(struct rtl8139 *r) {
	return (pci_mmio_read8(r->mmio_base, CR) & CR_RX_BUF_EMPTY) != 0;
}

static void tx_write(struct rtl8139 *r, uint32_t data, size_t size) {
	pci_mmio_write32(r->mmio_base, TAD0 + r->tx_slot * 4, data);
	pci_mmio_write32(r->mmio_base, TSD0 + r->tx_slot * 4, size);

	r->tx_slot++;
	r->tx_slot %= 4;
}

void rtl8139_send(struct rtl8139 *r, void *data, size_t size) {
	verbose_printf("rtl8139: send_packet: len %zu\n", size);

	while ((tx_status(r) & TX_OWN) != 0) {
		// this descriptor is still busy. the nic will
		// set the own bit when it's done with it.
		asm volatile("pause");
	}

	phys_addr_t data_phy = vmm_virt_to_phy((uintptr_t)data);
	phys_addr_t data_phy_end = vmm_virt_to_phy((uintptr_t)data + size);
	if (data_phy_end - data_phy != size) {
		printf("rtl8139: send_packet: data spans multiple pages\n");
		return;
	}
	tx_write(r, data_phy, size);
}

uint16_t *read_packet_header(struct rtl8139 *r) {
	return (uint16_t *)(r->rx_buffer + r->rx_index);
}

ssize_t rtl8139_receive(struct rtl8139 *r, void *data, size_t len) {
	uint16_t *header = read_packet_header(r);
	uint16_t flags = header[0];
	uint16_t length = header[1];

	verbose_printf("rtl8139: recv_packet: flags %04x len %i\n", flags, length);

	ssize_t result = -1;
	if ((flags & 1) == 0) {
		printf("rtl8139: bad packet\n");
	} else {
		size_t to_copy = MIN(len, length - 8);
		memcpy(data, r->rx_buffer + r->rx_index + 4, to_copy);
		result = (ssize_t)(to_copy);
	}

	r->rx_index += ROUND_UP(length + 4, 4);
	r->rx_index %= RX_BUFFER_SIZE;
	pci_mmio_write16(r->mmio_base, CAPR, r->rx_index - 0x10);

	return result;
}

void net_debug(int, const void *, size_t);

void print_packet(const void *data, size_t len) {
	printf("rtl8139: received packet of length %zu\n", len);
	net_debug(0, data, len);
}

void rtl8139_interrupt_handler(interrupt_frame *_, void *rtl) {
	struct rtl8139 *r = rtl;

	uint16_t int_flag = ack_interrupts(r);
	if (int_flag == 0) {
		// no interrupt to handle
		return;
	}

	if ((int_flag & INTR_RX_OK) != 0) {
		verbose_printf("rtl8139: rx ok\n");

		while (!rx_empty(r)) {
			char buffer[2048];
			ssize_t len = rtl8139_receive(r, buffer, sizeof(buffer));
#ifdef VERBOSE
			if (len > 0)
				print_packet(buffer, len);
#endif
		}

	} else if ((int_flag & INTR_RX_ERR) != 0) {
		verbose_printf("rtl8139: rx err\n");
	} else if ((int_flag & INTR_TX_OK) != 0) {
		verbose_printf("rtl8139: tx ok\n");
	} else if ((int_flag & INTR_TX_ERR) != 0) {
		verbose_printf("rtl8139: tx err\n");
	} else if (int_flag == 0) {
		// no interrupt to handle
	} else {
		verbose_printf("rtl8139: unknown interrupt\n");
	}

	pci_mmio_write16(r->mmio_base, ISR, int_flag);
}

void rtl_test() {
	pci_address_t pci_address = pci_find_device_by_id(0x10EC, 0x8139);
	if (pci_address != 0) {
		struct rtl8139 *r = calloc(1, sizeof(struct rtl8139));
		rtl8139_init(r, pci_find_device_by_id(0x10EC, 0x8139));

		int irq = pci_read8(pci_address, PCI_INTERRUPT_LINE);

		irq_install(irq, rtl8139_interrupt_handler, r);

		unsigned char ethernet_frame[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0x52, 0x54, 0x00, 0x12, 0x34, 0x56, 0x08, 0x06, 0x00, 0x01, 0x08,
			0x00, 0x06, 0x04, 0x00, 0x01, 0x52, 0x54, 0x00, 0x12, 0x34, 0x56,
			0x0a, 0x00, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a,
			0x00, 0x02, 0x02 };
		rtl8139_send(r, ethernet_frame, sizeof(ethernet_frame));
	}
}
