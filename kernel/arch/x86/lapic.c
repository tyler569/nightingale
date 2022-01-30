#include <basic.h>
#include <ng/vmm.h>
#include <stdatomic.h>
#include <x86/cpu.h>

#define LAPIC_ID 0x020
#define LAPIC_VERSION 0x030
#define LAPIC_TPR 0x080     // Task Priority Register
#define LAPIC_APR 0x090     // Arbitration Priority Register
#define LAPIC_PPR 0x0A0     // Processor Priority Register
#define LAPIC_EOI 0x0B0     // End Of Interrupt Register
#define LAPIC_RRD 0x0C0     // Remote Read Register
#define LAPIC_LDR 0x0D0     // Logical Destination Register
#define LAPIC_DFR 0x0E0     // Destination Format Register
#define LAPIC_SVR 0x0F0     // Spurious Interrupt Vector Register
#define LAPIC_ISR 0x100 // -> 0x170  In-Service Register
#define LAPIC_TMR 0x180 // -> 0x1F0  Trigger Mode Register
#define LAPIC_IRR 0x200 // -> 0x270  Interrupt Request Register
#define LAPIC_ESR 0x280     // Error Status Register
#define LAPIC_LVT_CMCI 0x2F0 // LVT Corrected Machine Check Interrupt Register
// intentionally 2 -> 300 -- writes go to 310 first
#define LAPIC_ICR2 0x300    // Interrupt Command Register
#define LAPIC_ICR1 0x310    // Interrupt Command Register
#define LAPIC_LVT_TIMER 0x320   // LVT Timer Register
#define LAPIC_LVT_THERMAL 0x330 // LVT Thermal Sensor Register
#define LAPIC_LVT_PMC 0x340     // LVT Performance Monitoring Counter Register
#define LAPIC_LVT_LINT0 0x350   // LVT LINT0 Register
#define LAPIC_LVT_LINT1 0x360   // LVT LINT1 Register
#define LAPIC_LVT_ERROR 0x370   // LVT Error Register
#define LAPIC_TIMER_ICR 0x380   // LVT Timer Initial Count Register
#define LAPIC_TIMER_CCR 0x390   // LVT Timer Current Count Register
#define LAPIC_TIMER_DCR 0x3E0   // LVT Timer Divide Configuration Register

#define IPI_NORMAL 0
#define IPI_LOWPRI 1
#define IPI_SMI 2
#define IPI_NMI 4
#define IPI_INIT 5
#define IPI_SIPI 6

#define DESTINATION_SELF 1
#define DESTINATION_ALL 2
#define DESTINATION_ALL_OTHER 3


uint32_t lapic_linear_address = 0xFEE0000;
uintptr_t lapic_mapped_address;

static void lapic_mmio_w(int reg, uint32_t value) {
    atomic_store((_Atomic uint32_t *)(lapic_mapped_address + reg), value);
}

static uint32_t lapic_mmio_r(int reg) {
    return atomic_load((_Atomic uint32_t *)(lapic_mapped_address + reg));
}

void lapic_init() {
    lapic_mapped_address = vmm_mapobj_iwi(lapic_linear_address, 0x1000);
    lapic_mmio_w(LAPIC_ESR, 0);
    lapic_mmio_w(LAPIC_SVR, 0xFF);

    uint64_t lapic_base_msr = rdmsr(IA32_APIC_BASE);
    wrmsr(IA32_APIC_BASE, lapic_base_msr | (1 << 11));
}

void lapic_eoi(int interrupt_number) {
    (void)interrupt_number;
    lapic_mmio_w(LAPIC_EOI, 0);
}

static void lapic_await_delivery() {
    while (lapic_mmio_r(LAPIC_ICR2) & (1 << 12)) {
        asm volatile ("pause");
    }
}

static void lapic_send_ipi_raw(uint32_t icr, int destination_processor) {
    lapic_mmio_w(LAPIC_ICR1, destination_processor << 24);
    lapic_mmio_w(LAPIC_ICR2, icr);
    lapic_await_delivery();
}

void lapic_send_init(int destination_processor) {
    uint32_t command = (
            (IPI_INIT << 8) | 
            (1 << 12) | // delivery status
            (1 << 14)   // assert
    );
    lapic_send_ipi_raw(command, destination_processor);
    for (volatile int x = 0; x < 100000; x++) asm volatile ("pause");
    command = (
            (IPI_INIT << 8) |
            (1 << 12) |
            (1 << 15)   // deassert
    );
    lapic_send_ipi_raw(command, destination_processor);
}

void lapic_send_ipi(int type, int vector, int destination_processor) {
    uint32_t command = (
            vector |
            (type << 8) |
            (1 << 14)
    );
    lapic_send_ipi_raw(command, destination_processor);
}
