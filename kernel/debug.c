#include "assert.h"
#include "chacha.h"
#include "rng.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "sys/arch.h"
#include "sys/mem.h"

void hexdump(const void *data, size_t len);

[[noreturn]] void panic(const char *msg, ...) {
	va_list args;
	va_start(args, msg);

	printf("PANIC: ");
	vprintf(msg, args);
	va_end(args);

	printf("\n");

	halt_forever();
}

void run_smoke_tests() {
	printf("Smoke tests:\n");

	// printf ("  Interrupt\n");
	// debug_trap ();

	printf("  Allocator\n");
	{
#define ALLOCS 512
		struct allocation {
			size_t size;
			void *ptr;
			char fill;
			bool freed;
		};
		struct allocation allocations[ALLOCS];
		size_t rounds = 4;

		for (size_t round = 0; round < rounds; round++) {
			for (size_t i = 0; i < ALLOCS; i++) {
				size_t parameter = random_u64();
				size_t len = (1 << ((parameter & 0xf) + 1));
				len += (parameter >> 4) % len;

				// printf ("Allocating %zu bytes\n", len);
				allocations[i].ptr = kmem_alloc(len);
				allocations[i].size = len;
				allocations[i].fill = (char)(random_u64() & 0xff);
				allocations[i].freed = false;
				memset(allocations[i].ptr, allocations[i].fill, len);
			}

			for (size_t i = ALLOCS - 1; i-- > 0;) {
				assert(!allocations[i].freed);
				for (size_t j = 0; j < allocations[i].size; j++)
					assert(
						((char *)allocations[i].ptr)[j] == allocations[i].fill);

				// printf ("Freeing %p %zu bytes\n", allocations[i].ptr,
				//         allocations[i].size);
				kmem_free(allocations[i].ptr);
				allocations[i].freed = true;
			}
		}
	}

	printf("  Chacha\n");
	{
		// test vector2 from RFC 7539
		struct chacha cc1 = {
			1,
			{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
				19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 },
			{ 0, 0, 0, 9, 0, 0, 0, 0x4a, 0, 0, 0, 0 },
		};

		uint8_t data[64] = { 0 };
		xor_chacha(&cc1, data, sizeof(data));

		assert(data[0] == 0x10 && data[63] == 0x4e);

		unsigned char data2[]
			= "Ladies and Gentlemen of the class of '99: If I could offer "
			  "you only one tip for the future, sunscreen would be it.";

		struct chacha cc2 = {
			1,
			{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
				19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 },
			{ 0, 0, 0, 0, 0, 0, 0, 0x4a, 0, 0, 0, 0 },
		};

		xor_chacha(&cc2, data2, sizeof(data2) - 1);

		assert(data2[0] == 0x6e && data2[1] == 0x2e && data2[2] == 0x35);
		assert(data2[112] == 0x87 && data2[113] == 0x4d);
	}
}
