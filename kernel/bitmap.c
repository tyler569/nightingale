
#include <basic.h>
#include <nc/stdio.h>
#include <nc/stdlib.h>
#include <nc/string.h>

// Note that bitmap requires external synchronization!
struct bitmap {
    size_t bitmap_len;
    unsigned char bitmap[];
};

struct bitmap *bitmap_new(size_t entries) {
    size_t bitmap_len = round_up(entries, 8) / 8;
    size_t alloc = sizeof(struct bitmap) + bitmap_len;
    struct bitmap *bitmap = zmalloc(alloc);

    bitmap->bitmap_len = bitmap_len;

    return bitmap;
}

static int first_free_in_char(char c) {
    for (int i=0; i<8; i++) {
        if ((c & (1 << i)) == 0) {
            return i;
        }
    }
    return 0;
}

static char set_bit(char c, int bit) {
    return c | (1 << bit);
}

static char clear_bit(char c, int bit) {
    return c & ~(1 << bit);
}

static char bitmap_at(struct bitmap *bitmap, int offset) {
    return bitmap->bitmap[offset];
}

static char bitmap_set(struct bitmap *bitmap, int offset, char c) {
    return bitmap->bitmap[offset] = c;
}

static int first_free_offset(struct bitmap *bitmap) {
    for (long i=0; i<bitmap->bitmap_len; i++) {
        if (bitmap->bitmap[i] == 0xFF)  continue;

        return i;
    }
    return -1;
}

static long first_free(struct bitmap *bitmap) {
    int offset = first_free_offset(bitmap);
    char v = bitmap_at(bitmap, offset);
    int bit = first_free_in_char(v);

    return offset * 8 + bit;
}

long bitmap_take(struct bitmap *bitmap) {
    int offset = first_free_offset(bitmap);
    if (offset < 0)  return offset;

    char v = bitmap_at(bitmap, offset);
    int bit = first_free_in_char(v);

    char new = set_bit(v, bit);
    bitmap_set(bitmap, offset, new);

    return offset * 8 + bit;
}

void bitmap_release(struct bitmap *bitmap, long bit) {
    int offset = bit / 8;
    int o_bit = bit % 8;
    char old = bitmap_at(bitmap, offset);
    char new = clear_bit(old, o_bit);
    bitmap_set(bitmap, offset, new);
}

