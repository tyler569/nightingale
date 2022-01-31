#include <basic.h>
#include <ng/vmm.h>
#include <stdatomic.h>
#include <stdio.h>
#include <x86/cpu.h>
#include <x86/lapic.h>

#define DESTINATION_SELF 1
#define DESTINATION_ALL 2
#define DESTINATION_ALL_OTHER 3


uint32_t lapic_linear_address = 0xFEE0000;
uintptr_t lapic_mapped_address;

static void lapic_mmio_w(int reg, uint32_t value) {
    printf("lapic mmio write: %#010X -> register %#X\n", value, reg);
    atomic_store((_Atomic uint32_t *)(lapic_mapped_address + reg), value);
    // *(volatile uint32_t *)(lapic_mapped_address + reg) = value;
}

static uint32_t lapic_mmio_r(int reg) {
    return atomic_load((_Atomic uint32_t *)(lapic_mapped_address + reg));
    // return *(volatile uint32_t *)(lapic_mapped_address + reg);
}

void lapic_init() {
    lapic_mapped_address = vmm_mapobj_iwi(lapic_linear_address, 0x1000);

    uint64_t lapic_base_msr = rdmsr(IA32_APIC_BASE);
    wrmsr(IA32_APIC_BASE, lapic_base_msr | (1 << 11));

    lapic_mmio_w(LAPIC_ESR, 0);
    lapic_mmio_w(LAPIC_SVR, 0x1FF);
}

void lapic_eoi(int interrupt_number) {
    (void)interrupt_number;
    lapic_mmio_w(LAPIC_EOI, 0);
}

static void lapic_await_delivery() {
    while (lapic_mmio_r(LAPIC_ICR1) & (1 << 12)) {
        asm volatile ("pause");
    }
}

static void lapic_send_ipi_raw(uint32_t icr, int destination_processor) {
    lapic_mmio_w(LAPIC_ICR2, destination_processor << 24);
    lapic_mmio_w(LAPIC_ICR1, icr);
    for (volatile int x = 0; x < 10000; x++) asm volatile ("pause");
    lapic_await_delivery();
}

void lapic_send_init(int destination_processor) {
    uint32_t command = (
            (IPI_INIT << 8) | 
            (1 << 12) |
            (1 << 14) | // assert
            (1 << 15) | // level triggered
            (3 << 18)   // all except self
    );
    lapic_send_ipi_raw(command, 0);// destination_processor);
    for (volatile int x = 0; x < 100000; x++) asm volatile ("pause");
    command = (
            (IPI_INIT << 8) |
            (1 << 12) |
            (1 << 15) |
            (3 << 18)
    );
    lapic_send_ipi_raw(command, 0); // destination_processor);
}

void lapic_send_ipi(int type, int vector, int destination_processor) {
    uint32_t command = (
            vector |
            (type << 8)
            // (1 << 14)
    );
    lapic_send_ipi_raw(command, destination_processor);
}
