
#ifndef NIGHTINGALE_FS_CHAR_DEVICES_H
#define NIGHTINGALE_FS_CHAR_DEVICES_H

#include <basic.h>
#include <stddef.h>
#include <stdint.h>
#include <print.h>
#include <vector.h>
#include <syscall.h>
#include <ringbuf.h>
#include "vfs.h"

ssize_t dev_zero_read(struct fs_node *n, void *data_, size_t len);
ssize_t dev_null_write(struct fs_node *n, const void *data, size_t len);
ssize_t dev_inc_read(struct fs_node *n, void *data_, size_t len);
ssize_t serial_write(struct fs_node *n, const void *data_, size_t len);
ssize_t file_buf_read(struct fs_node *n, void *data_, size_t len);

#endif

