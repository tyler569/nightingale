#include <ng/x86/cpu.h>

// static uint32_t max_cpuid = 0; // FIXME cache
static uint32_t max_cpuid() {
	uint32_t out[4];
	cpuid(0, 0, out);
	return out[_RAX];
}

#define checked_cpuid(L, SL, O) \
	do { \
		if ((L) >= max_cpuid()) { \
			return false; \
		} \
		cpuid(L, SL, O); \
	} while (0);

bool supports_feature(enum x86_feature feature) {
	uint32_t out[4];

	switch (feature) {
	case _X86_FSGSBASE:
		checked_cpuid(7, 0, out);
		return out[_RBX] & 1;
	default:
		return false;
	}
}
