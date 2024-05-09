#include <ctype.h>
#include <hexdump.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>

#define BUFFER_SIZE 4096

char buffer[BUFFER_SIZE] = {};

int main() {
	size_t n;
	size_t carryover = 0;
	uintptr_t total = 0;

	setvbuf(stdin, nullptr, _IOFBF, 0);
	while ((n = fread(buffer + carryover, 1, BUFFER_SIZE - carryover, stdin))
		> 0) {
		n += carryover;
		size_t available = ROUND_DOWN(n, 16);
		carryover = n % 16;
		if (available > 0)
			hexdump_addr(buffer, available, total);
		total += available;
		if (carryover != 0) {
			memmove(buffer, buffer + available, carryover);
		}
	}
	hexdump_addr(buffer, carryover, total);
	return 0;
}
