
#pragma once
#ifndef NIGHTINGALE_BUF_H
#define NIGHTINGALE_BUF_H

#include <basic.h>
#include <stdint.h>
#include <stddef.h>

struct buf {
    void *data;
    size_t len;
    size_t size;
};

struct buf *new_buf(size_t size);
size_t buf_put(struct buf *buf, const void *data, size_t len);
size_t buf_get(struct buf *buf, void *data, size_t len);
void del_buf(struct buf *buf);

#endif

