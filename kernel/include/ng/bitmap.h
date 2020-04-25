
#pragma once

#include <basic.h>
#include <stddef.h>

// Note that bitmap requires external synchronization!
struct bitmap {
    size_t bitmap_len;
    unsigned char bitmap[];
};

struct bitmap *bitmap_new_early(size_t entries);
struct bitmap *bitmap_new(size_t entries);

long bitmap_take(struct bitmap *bitmap);
void bitmap_release(struct bitmap *bitmap, long bit);

