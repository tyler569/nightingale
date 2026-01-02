#include <ng/vmm.h>
#include <ng/x86/apic.h>
#include <ng/x86/cpu.h>
#include <ng/x86/pit.h>
#include <stdio.h>

#define DESTINATION_SELF 1
#define DESTINATION_ALL 2
#define DESTINATION_ALL_OTHER 3

#define PIT_CH0 0x40
#define PIT_CMD 0x43
#define CHANNEL_0 0x00
#define ACCESS_HILO 0x30
#define MODE_0 0x00

#define CALIBRATION_MS 10
#define PIT_FREQUENCY 1193182
#define TARGET_HZ 100

#define DELAY(usec) \
	do { \
		for (volatile int x = 0; x < (usec) * 10; x++) \
			asm volatile("pause"); \
	} while (0)

static const uint32_t lapic_linear_address = 0xFEE00000;
static const uintptr_t lapic_mapped_address = 0xFFFF8000FEE00000;

static void lapic_mmio_w(int reg, uint32_t value) {
	*(volatile uint32_t *)(lapic_mapped_address + reg) = value;
}

static uint32_t lapic_mmio_r(int reg) {
	return *(volatile uint32_t *)(lapic_mapped_address + reg);
}

static void lapic_init_timer();

void lapic_init() {
	vmm_map(lapic_mapped_address, lapic_linear_address, PAGE_WRITEABLE);

	uint64_t lapic_base_msr = rdmsr(IA32_APIC_BASE);
	wrmsr(IA32_APIC_BASE, lapic_base_msr | (1 << 11));

	lapic_mmio_w(LAPIC_ESR, 0);
	lapic_mmio_w(LAPIC_SVR, 0x1FF);

	lapic_mmio_w(LAPIC_LVT_LINT0, 0x00008700);
	lapic_mmio_w(LAPIC_LVT_LINT1, 0x00008400);

	lapic_init_timer();
}

void lapic_eoi(int interrupt_number) {
	(void)interrupt_number;
	lapic_mmio_w(LAPIC_EOI, 0);
}

static void lapic_await_delivery() {
	while (lapic_mmio_r(LAPIC_ICR1) & (1 << 12)) {
		asm volatile("pause");
	}
}

static void lapic_send_ipi_raw(uint32_t icr, int destination_processor) {
	lapic_mmio_w(LAPIC_ICR2, destination_processor << 24);
	lapic_mmio_w(LAPIC_ICR1, icr);
	DELAY(1000);
	lapic_await_delivery();
}

void lapic_send_init(int destination_processor) {
	uint32_t command = ((IPI_INIT << 8) |
		// (1 << 12) |
		(1 << 14) | // assert
		(1 << 15) | // level triggered
		(3 << 18) // all except self
	);
	lapic_send_ipi_raw(command, 1);
	DELAY(1000);
	command = ((IPI_INIT << 8) | (1 << 15) | (3 << 18));
	lapic_send_ipi_raw(command, 1);
}

void lapic_send_ipi(int type, int vector, int destination_processor) {
	uint32_t command = (vector | (type << 8));
	lapic_send_ipi_raw(command, destination_processor);
}

static uint32_t lapic_timer_frequency = 0;
static uint32_t lapic_ticks_per_tick = 0;

static void lapic_init_timer() {
	// Calculate PIT ticks for 10ms calibration window
	uint32_t pit_ticks = (PIT_FREQUENCY * CALIBRATION_MS) / 1000;

	// Pre-configure LAPIC timer (but don't start it yet)
	lapic_mmio_w(LAPIC_LVT_TIMER, 0x10000); // One-shot, masked
	lapic_mmio_w(LAPIC_TIMER_DCR, 0);       // Divide by 2

	// Set up PIT for one-shot countdown (MODE_0 = terminal count)
	outb(PIT_CMD, CHANNEL_0 | ACCESS_HILO | MODE_0);
	outb(PIT_CH0, pit_ticks & 0xFF);
	outb(PIT_CH0, pit_ticks >> 8);

	// Start LAPIC timer immediately after PIT to minimize delay
	lapic_mmio_w(LAPIC_TIMER_ICR, 0xFFFFFFFF); // Max count - this starts the timer

	// Poll PIT until it reaches zero (with timeout)
	uint32_t timeout = 10000000;
	uint16_t current;
	while (timeout-- > 0) {
		current = pit_read_count();
		if (current == 0 || current > pit_ticks) {
			break; // Terminal count reached
		}
		asm volatile("pause");
	}

	if (timeout == 0) {
		printf("LAPIC: calibration timeout, using fallback\n");
		lapic_mmio_w(LAPIC_LVT_TIMER, 0x20020);
		lapic_mmio_w(LAPIC_TIMER_ICR, 100000);
		return;
	}

	// Read how many LAPIC ticks elapsed
	uint32_t lapic_end = lapic_mmio_r(LAPIC_TIMER_CCR);
	uint32_t lapic_elapsed = 0xFFFFFFFF - lapic_end;

	// Debug output
	printf("LAPIC calibration: PIT programmed=%u, final=%u\n", pit_ticks, current);
	printf("LAPIC calibration: start=0xFFFFFFFF, end=%u, elapsed=%u\n",
	       lapic_end, lapic_elapsed);

	// Calculate LAPIC frequency and ticks needed for 100 Hz (use 64-bit to avoid overflow)
	lapic_timer_frequency = ((uint64_t)lapic_elapsed * 1000) / CALIBRATION_MS;
	lapic_ticks_per_tick = lapic_timer_frequency / TARGET_HZ;

	// Sanity checks
	if (lapic_timer_frequency < 1000000 || lapic_timer_frequency > 1000000000) {
		printf("LAPIC: WARNING: unusual frequency %u Hz, using fallback\n",
		       lapic_timer_frequency);
		lapic_ticks_per_tick = 100000;
	} else if (lapic_ticks_per_tick < 100 || lapic_ticks_per_tick > 10000000) {
		printf("LAPIC: WARNING: unusual ticks_per_tick %u, using fallback\n",
		       lapic_ticks_per_tick);
		lapic_ticks_per_tick = 100000;
	} else {
		printf("LAPIC: calibrated timer frequency: %u Hz\n", lapic_timer_frequency);
		printf("LAPIC: ticks per 1/%d second: %u\n", TARGET_HZ, lapic_ticks_per_tick);
	}

	// Configure for periodic operation at 100 Hz
	lapic_mmio_w(LAPIC_LVT_TIMER, 0x20020); // Periodic mode, vector 0x20
	lapic_mmio_w(LAPIC_TIMER_DCR, 0);        // Keep divide by 2
	lapic_mmio_w(LAPIC_TIMER_ICR, lapic_ticks_per_tick);
}
