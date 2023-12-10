#include <fcntl.h>
#include <ng/common.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inode.h>
#include <ng/fs/pipe.h>
#include <ng/signal.h>
#include <ng/sync.h>
#include <stdlib.h>
#include <string.h>

#define wait_on wq_block_on
#define wake_from wq_notify_all

extern inode_operations pipe_ops;
extern file_operations pipe_file_ops;

inode *new_pipe(void)
{
    inode *inode = new_inode(initfs_file_system, S_IFIFO | 0777);
    inode->capacity = 16384;
    inode->data = malloc(inode->capacity);
    inode->ops = &pipe_ops;
    inode->file_ops = &pipe_file_ops;
    inode->type = FT_PIPE;
    return inode;
}

int pipe_open(inode *pipe, file *file)
{
    if (pipe->is_anon_pipe)
        return 0;

    if (file->flags & O_WRONLY && file->flags & O_RDONLY)
        return -EINVAL;

    if (file->flags & O_WRONLY && pipe->read_refcnt == 0)
        wait_on(&pipe->write_queue);

    if (file->flags & O_RDONLY && pipe->write_refcnt == 0)
        wait_on(&pipe->read_queue);

    if (file->flags & O_WRONLY && pipe->write_refcnt == 0)
        wake_from(&pipe->read_queue);

    if (file->flags & O_RDONLY && pipe->read_refcnt == 0)
        wake_from(&pipe->write_queue);

    return 0;
}

int pipe_close(inode *pipe, file *file)
{
    if (pipe->write_refcnt == 0)
        wake_from(&pipe->read_queue);
    if (pipe->read_refcnt == 0)
        wake_from(&pipe->write_queue);
    return 0;
}

inode_operations pipe_ops = {
    .open = pipe_open,
    .close = pipe_close,
};

ssize_t pipe_read(file *file, char *buffer, size_t len)
{
    inode *inode = file->inode;
    while (inode->len == 0 && inode->write_refcnt)
        wait_on(&inode->read_queue);

    size_t to_read = MIN(len, inode->len);
    memcpy(buffer, inode->data, to_read);
    memmove(inode->data, PTR_ADD(inode->data, to_read), inode->len - to_read);
    inode->len -= to_read;
    wake_from(&inode->write_queue);
    return to_read;
}

ssize_t pipe_write(file *file, const char *buffer, size_t len)
{
    inode *inode = file->inode;

    if (!inode->read_refcnt) {
        signal_self(SIGPIPE);
        return 0;
    }

    while (inode->len == inode->capacity && inode->read_refcnt)
        wait_on(&inode->write_queue);

    if (!inode->read_refcnt) {
        signal_self(SIGPIPE);
        return 0;
    }

    size_t to_write = MIN(len, inode->capacity - inode->len);
    memcpy(PTR_ADD(inode->data, inode->len), buffer, to_write);
    inode->len += to_write;
    wake_from(&inode->read_queue);
    return to_write;
}

file_operations pipe_file_ops = {
    .read = pipe_read,
    .write = pipe_write,
};
