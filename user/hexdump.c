#include <ctype.h>
#include <ng/common.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 4096

static char dump_byte_char(char c) { return isprint(c) ? c : '.'; }

static void print_byte_char_line(const char *c, size_t remaining_len) {
	for (int i = 0; i < remaining_len; i++) {
		printf("%c", dump_byte_char(c[i]));
	}
}

static void hexdump_line(
	const void *data, size_t remaining_len, uintptr_t base_address) {
	printf("%08lx: ", base_address);
	size_t i;
	for (i = 0; i < remaining_len; i++) {
		printf("%02hhx ", ((const char *)data)[i]);
		if (i == 7)
			printf(" ");
	}
	for (; i < 16; i++) {
		printf("   ");
		if (i == 7)
			printf(" ");
	}
	printf("  ");
	print_byte_char_line(data, remaining_len);
	printf("\n");
}

void hexdump(const void *data, size_t len, uintptr_t base_address) {
	for (size_t i = 0; i < len; i += 16) {
		hexdump_line(data + i, MIN(len - i, 16), base_address + i);
	}
}

char buffer[BUFFER_SIZE] = {};

int main() {
	size_t n;
	size_t carryover = 0;
	uintptr_t total = 0;

	setvbuf(stdin, NULL, _IOFBF, 0);
	while ((n = fread(buffer + carryover, 1, BUFFER_SIZE - carryover, stdin))
		> 0) {
		n += carryover;
		size_t available = ROUND_DOWN(n, 16);
		carryover = n % 16;
		if (available > 0)
			hexdump(buffer, available, total);
		total += available;
		if (carryover != 0) {
			memmove(buffer, buffer + available, carryover);
		}
	}
	hexdump(buffer, carryover, total);
	return 0;
}
