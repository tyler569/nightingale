#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char **argv) {
	int request = 0;
	int sub = 0;

	if (argc >= 2)
		request = strtol(argv[1], nullptr, 0);
	if (argc >= 3)
		request = strtol(argv[2], nullptr, 0);

	unsigned id[4];
	cpuid(request, sub, id);
	printf("%08x %08x %08x %08x\n", id[0], id[1], id[2], id[3]);
}
