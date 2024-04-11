#include <assert.h>
#include <ng/common.h>
#include <ng/print.h>
#include <string.h>

struct buf {
	char *buffer;
	size_t len;
	size_t size;
};

void buf_write(struct writer w, const char *buf, size_t size) {
	struct buf *buffer = w.data;
	memcpy(buffer->buffer + buffer->len, buf,
		MIN(size, buffer->size - buffer->len));
	buffer->len += size;
}

struct writer_vtbl buf_vtbl = {
	.write = buf_write,
};

#define ASSERT_FMT(target, fmt, ...) \
	do { \
		buf.len = 0; \
		fprintf(w, fmt, ##__VA_ARGS__); \
		if (strcmp(buffer, target) != 0) { \
			printf("format error: wanted \"%s\", got \"%s\" on line %d\n", \
				target, buffer, __LINE__); \
		} \
		memset(buffer, 0, 256); \
	} while (0)

#define ASSERT_FMT_LIMIT(limit, target, fmt, ...) \
	do { \
		buf.len = 0; \
		fnprintf(w, limit, fmt, ##__VA_ARGS__); \
		if (strcmp(buffer, target) != 0) { \
			printf("format error: wanted \"%s\", got \"%s\" on line %d\n", \
				target, buffer, __LINE__); \
		} \
		memset(buffer, 0, 256); \
	} while (0)

void print_test() {
	char buffer[256] = {};
	struct buf buf = { .buffer = buffer, .len = 0, .size = sizeof(buffer) };
	struct writer w = { .data = &buf, .vtbl = &buf_vtbl };

	ASSERT_FMT("Hello, world!", "Hello, %s!", "world");
	ASSERT_FMT("Hello, 42!", "Hello, %d!", 42);
	ASSERT_FMT("Hello, 42!", "Hello, %i!", 42);
	ASSERT_FMT("Hello, 2a!", "Hello, %x!", 42u);
	ASSERT_FMT("Hello, 2A!", "Hello, %X!", 42u);
	ASSERT_FMT("Hello, 52!", "Hello, %o!", 42u);
	ASSERT_FMT("Hello, 42!", "Hello, %u!", 42u);
	ASSERT_FMT("Hello, *!", "Hello, %c!", 42);
	ASSERT_FMT("Hello, 101010!", "Hello, %b!", 42u);
	ASSERT_FMT("Hello, 42!", "Hello, %#i!", 42);
	ASSERT_FMT("Hello, 0x2a!", "Hello, %#x!", 42u);
	ASSERT_FMT("Hello, 0X2A!", "Hello, %#X!", 42u);
	ASSERT_FMT("Hello, 052!", "Hello, %#o!", 42u);
	ASSERT_FMT("Hello, 42!", "Hello, %#u!", 42u);
	ASSERT_FMT("Hello, 0b101010!", "Hello, %#b!", 42u);
	ASSERT_FMT("Hello, 0x1001!", "Hello, %p!", (void *)0x1001);

	ASSERT_FMT("Hello, 00000042!", "Hello, %08i!", 42);
	ASSERT_FMT("Hello, 0x00002a!", "Hello, %#08x!", 42u);

	ASSERT_FMT("Hello", "%3s", "Hello");
	ASSERT_FMT("Hel", "%.3s", "Hello");
	ASSERT_FMT("     Hello", "%10s", "Hello");
	ASSERT_FMT("Hello     ", "%-10s", "Hello");
	ASSERT_FMT("     Hello", "%10.5s", "Hello World");
	ASSERT_FMT("Hello     ", "%-10.5s", "Hello World");

	ASSERT_FMT("100 0x64 0144", "%d %#x %#o", 100, 100, 100);
	ASSERT_FMT("% %", "%% %s", "%");

	ASSERT_FMT("012345", "%.*s", 6, "0123456789");
	ASSERT_FMT("012345", "%.*s", 6, "012345");
	ASSERT_FMT("    012345", "%0*s", 10, "012345");
	ASSERT_FMT("0123456789", "%0*s", 10, "0123456789");
	ASSERT_FMT("          ", "%0*s", 10, "");

	ASSERT_FMT("1 1 1", "%i %i %i", 1, 1, 1);
	ASSERT_FMT("1 a 1", "%i %s %i", 1, "a", 1);
	ASSERT_FMT("1 a 1 aaa 1", "%i %s %i %.*s %i", 1, "a", 1, 3, "aaaaa", 1);

	ASSERT_FMT_LIMIT(10, "Hello, wo", "Hello, world!");
	ASSERT_FMT_LIMIT(10, "Hello, wo", "Hello, %s!", "world");
	ASSERT_FMT_LIMIT(10, "Hello, 42", "Hello, %d!", 42);
	ASSERT_FMT_LIMIT(9, "Hello, 4", "Hello, %i!", 42);
	ASSERT_FMT_LIMIT(12, "Hello, 0x00", "Hello, %#010x!", 1);

	ASSERT_FMT("reclaim (bl)    0000000000001000 00052000",
		"%-15s %016lx %08lx", "reclaim (bl)", (void *)0x1000, 0x52000);

	ASSERT_FMT("(nullptr)", "%p", nullptr);
	ASSERT_FMT("(null)", "%s", nullptr);
}
