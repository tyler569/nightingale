// #include <ng/event_log.h>
#include <ng/cpu.h>
#include <ng/init.h>
#include <ng/pci.h>
#include <ng/vmm.h>
#include <stdio.h>

static constexpr uintptr_t event_log_base = 0xffff'8200'0000'0000;
static bool is_init = false;
static volatile size_t *offset = (volatile size_t *)event_log_base;

void log_init() {
	uint32_t pci_addr = pci_find_device_by_id(0x1af4, 0x1110);
	if (pci_addr == 0xffff'ffff)
		return;

	printf("log_init: pci addr is: %#x\n", pci_addr);

	uintptr_t bar2 = pci_get_bar(pci_addr, 2) & ~0xf;

	printf("log_init: bar2 is %lx\n", bar2);

	vmm_map_range(event_log_base, bar2, 16 * 1024 * 1024,
		PAGE_WRITABLE | PAGE_WRITETHROUGH | PAGE_CACHEDISABLE);

	*offset = 16;
	is_init = true;
}
define_init(log_init, 4);

void log(uintptr_t a, uintptr_t b, uintptr_t c) {
	if (!is_init)
		return;

	uintptr_t *ptr = (uintptr_t *)(event_log_base + *offset);
	*offset += sizeof(uintptr_t) * 4;

	ptr[0] = a;
	ptr[1] = b;
	ptr[2] = c;
	ptr[3] = rdtsc();
}
