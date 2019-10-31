
#include <ng/basic.h>
#include <ng/print.h>

// TODO: replace this with printf when I add 0-padding.
void debug_print_mem(size_t cnt, void *mem_) {
        uint8_t *mem = mem_;
        for (size_t i = 0; i < cnt; i++) {
                printf("%02x ", mem[i]);
        }
}

void debug_dump(void *mem) {
        printf("128 bytes surrounding address: %#x\n", mem);

        uintptr_t base = (uintptr_t)mem & ~0x0f;
        for (int i = -64; i < 64; i += 16) {
                printf("%#010x : ", base + i);
                debug_print_mem(16, (void *)(base + i));
                printf("\n");
        }
}

void debug_dump_after(void *mem) {
        printf("128 bytes from address: %#x\n", mem);

        uintptr_t base = (uintptr_t)mem & ~0x0f;
        for (size_t i = 0; i < 128; i += 16) {
                printf("%#010x : ", base + i);
                debug_print_mem(16, (void *)(base + i));
                printf("\n");
        }
}

