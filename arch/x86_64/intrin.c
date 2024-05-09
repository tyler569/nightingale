#include "stdio.h"
#include "sys/spinlock.h"
#include "x86_64.h"

void halt_until_interrupt() { asm volatile("hlt"); }

[[noreturn]] void halt_forever() {
	while (true) {
		asm volatile("cli");
		asm volatile("hlt");
	}
}

[[noreturn]] void halt_forever_interrupts_enabled() {
	while (true)
		asm volatile("hlt");
}

#define DEFINE_PORT_IO(suffix, type) \
	void write_port_##suffix(uint16_t port, type value) { \
		asm volatile("out" #suffix " %0, %1" : : "a"(value), "Nd"(port)); \
	} \
\
	type read_port_##suffix(uint16_t port) { \
		type ret; \
		asm volatile("in" #suffix " %1, %0" : "=a"(ret) : "Nd"(port)); \
		return ret; \
	}

DEFINE_PORT_IO(b, uint8_t)
DEFINE_PORT_IO(w, uint16_t)
DEFINE_PORT_IO(l, uint32_t)

void write_msr(uint32_t msr_id, uint64_t value) {
	uint32_t low = value & 0xffffffff;
	uint32_t high = value >> 32;
	asm volatile("wrmsr" : : "c"(msr_id), "a"(low), "d"(high));
}

uint64_t read_msr(uint32_t msr_id) {
	uint32_t low, high;
	asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr_id));
	return ((uint64_t)high << 32) | low;
}

void write_fsbase(uintptr_t value) {
	// asm volatile ("wrfsbase %0" : : "r"(value));
	write_msr(IA32_FS_BASE, value);
}

void write_gsbase(uintptr_t value) {
	// asm volatile ("wrgsbase %0" : : "r"(value));
	write_msr(IA32_GS_BASE, value);
}

uint64_t read_cr2() {
	uint64_t ret;
	asm volatile("mov %%cr2, %0" : "=r"(ret));
	return ret;
}

uint64_t read_cr4() {
	uint64_t ret;
	asm volatile("mov %%cr4, %0" : "=r"(ret));
	return ret;
}

void write_cr4(uint64_t value) { asm volatile("mov %0, %%cr4" : : "r"(value)); }

void write_e9(char c) { write_port_b(0xe9, c); }

ssize_t write_debug(FILE *, const void *str, size_t len) {
	static spin_lock_t lock;

	spin_lock(&lock);

	for (size_t i = 0; i < len && ((char *)str)[i]; i++)
		write_e9(((char *)str)[i]);

	spin_unlock(&lock);

	return (ssize_t)len;
}

void relax_busy_loop() { asm volatile("pause"); }

void debug_trap() { asm volatile("int3"); }
