#pragma once

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/frame_x86_64.h>

#define IA32_APIC_BASE 27

#define NIRQS 16

typedef uint16_t port_addr_t;

#define INTERRUPT_ENABLE 0x200
#define TRAP_FLAG 0x100

enum x86_feature {
	_X86_FSGSBASE,
	_X86_SMEP,
	_X86_SMAP,
};

BEGIN_DECLS

bool supports_feature(enum x86_feature feature);
[[noreturn]] void longjump_kcode(uintptr_t ip, uintptr_t sp);

static inline uint64_t rdtsc() { return __builtin_ia32_rdtsc(); }

static inline uintptr_t cr3() {
	uintptr_t cr3 = 0;
	asm volatile("mov %%cr3, %0" : "=a"(cr3));
	return cr3;
}

enum {
	_RAX,
	_RBX,
	_RCX,
	_RDX,
};

static inline void cpuid(uint32_t a, uint32_t c, uint32_t out[4]) {
	asm("cpuid"
		: "=a"(out[_RAX]), "=b"(out[_RBX]), "=c"(out[_RCX]), "=d"(out[_RDX])
		: "0"(a), "2"(c));
}

static inline int cpunum() {
	uint32_t out[4];
	cpuid(1, 0, out);
	return out[_RBX] >> 24;
}

static inline int cpu_id() {
	uint32_t out[4];
	cpuid(1, 0, out);
	return out[_RBX] >> 24;
}

static inline void enable_bits_cr4(uintptr_t bitmap) {
	uintptr_t tmp;
	asm volatile("mov %%cr4, %%rax\n\t"
				 "or %0, %%rax\n\t"
				 "mov %%rax, %%cr4\n\t"
				 :
				 : "r"(bitmap)
				 : "rax");
}

static inline uint8_t inb(port_addr_t port) {
	uint8_t result;
	asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
	return result;
}

static inline void outb(port_addr_t port, uint8_t data) {
	asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

static inline uint16_t inw(port_addr_t port) {
	uint16_t result;
	asm volatile("inw %1, %0" : "=a"(result) : "Nd"(port));
	return result;
}

static inline void outw(port_addr_t port, uint16_t data) {
	asm volatile("outw %0, %1" : : "a"(data), "Nd"(port));
}

static inline uint32_t ind(port_addr_t port) {
	uint32_t result;
	asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
	return result;
}

static inline void outd(port_addr_t port, uint32_t data) {
	asm volatile("outl %0, %1" : : "a"(data), "Nd"(port));
}

static inline void invlpg(uintptr_t address) {
	asm volatile("invlpg (%0)" : : "b"(address) : "memory");
}

static inline void flush_tlb() {
	long temp = 0;
	asm volatile("mov %%cr3, %0 \n\t"
				 "mov %0, %%cr3 \n\t"
				 : "=r"(temp)
				 : "0"(temp));
}

static inline uint64_t rdmsr(uint32_t msr_id) {
	uint32_t a, d;
	asm volatile("rdmsr" : "=a"(a), "=d"(d) : "c"(msr_id));
	return ((uint64_t)d << 32) + a;
}

static inline void wrmsr(uint32_t msr_id, uint64_t value) {
	uint32_t a = value, d = value >> 32;
	asm volatile("wrmsr" : : "c"(msr_id), "a"(a), "d"(d));
}

static inline void set_tls_base(void *tlsbase) {
	extern int have_fsgsbase;
	if (have_fsgsbase)
		asm volatile("wrfsbase %0" ::"r"(tlsbase));
	else
		wrmsr(0xC0000100, (uintptr_t)tlsbase);
}

static inline void set_gs_base(void *gsbase) {
	extern int have_fsgsbase;
	if (have_fsgsbase)
		asm volatile("wrgsbase %0" ::"r"(gsbase));
	else
		wrmsr(0xC0000101, (uintptr_t)gsbase);
}

static inline void *get_gs_base() {
	extern int have_fsgsbase;
	void *gsbase;
	if (have_fsgsbase)
		asm volatile("rdgsbase %0" : "=r"(gsbase));
	else
		gsbase = (void *)rdmsr(0xC0000101);
	return gsbase;
}

static inline uintptr_t dr6() {
	uintptr_t result;
	asm volatile("mov %%dr6, %0\n\t" : "=r"(result));
	return result;
}

END_DECLS
