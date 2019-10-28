
#ifndef NIGHTINGALE_FS_MEMBUF_H
#define NIGHTINGALE_FS_MEMBUF_H

#include <ng/basic.h>
#include <ng/fs.h>

ssize_t membuf_read(struct open_fd *n, void *data, size_t len);
ssize_t membuf_write(struct open_fd *n, const void *data, size_t len);
off_t membuf_seek(struct open_fd *n, off_t offset, int whence);

#endif
