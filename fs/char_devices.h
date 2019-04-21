
#ifndef NIGHTINGALE_FS_CHAR_DEVICES_H
#define NIGHTINGALE_FS_CHAR_DEVICES_H

#include <ng/basic.h>
#include <stddef.h>
#include <stdint.h>
#include <ng/print.h>
#include <ds/vector.h>
#include <ng/syscall.h>
#include <ds/ringbuf.h>
#include <fs/vfs.h>

ssize_t dev_zero_read(struct fs_node *n, void *data_, size_t len);
ssize_t dev_null_write(struct fs_node *n, const void *data, size_t len);
ssize_t dev_inc_read(struct fs_node *n, void *data_, size_t len);
ssize_t serial_write(struct fs_node *n, const void *data_, size_t len);
ssize_t file_buf_read(struct fs_node *n, void *data_, size_t len);

#endif

