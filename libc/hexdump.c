#include <ctype.h>
#include <hexdump.h>
#include <ng/common.h>
#include <stdio.h>

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

void hexdump_addr(const void *data, size_t len, uintptr_t base_address) {
	for (size_t i = 0; i < len; i += 16) {
		hexdump_line(data + i, MIN(len - i, 16), base_address + i);
	}
}

void hexdump(const void *data, size_t len) { hexdump_addr(data, len, 0); }
