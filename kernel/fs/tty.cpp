#include <ng/common.h>
#include <ng/fs/file.h>
#include <ng/fs/inode.h>
#include <ng/serial.h>
#include <ng/tty.h>

#define wait_on wq_block_on

static tty *file_tty(file *f)
{
    int minor = f->inode->device_minor;
    if (minor >= 32 || minor < 0)
        return (tty *)TO_ERROR(-ENODEV);
    tty *t = global_ttys[minor];
    if (!t)
        return (tty *)TO_ERROR(-ENODEV);
    return t;
}

ssize_t tty_write(file *file, const char *data, size_t len)
{
    tty *tty = file_tty(file);
    if (IS_ERROR(tty))
        return ERROR(tty);

    serial_device *dev = tty->serial_device;
    if (!dev)
        return -ENODEV; // ?

    dev->ops->write_string(dev, data, len);
    return len;
}

ssize_t tty_read(file *file, char *data, size_t len)
{
    tty *tty = file_tty(file);
    if (IS_ERROR(tty))
        return ERROR(tty);

    if (tty->signal_eof) {
        tty->signal_eof = false;
        return 0;
    }

    size_t n_read = ring_read(&tty->ring, data, len);
    if (n_read != 0)
        return n_read;

    wait_on(&tty->read_queue);

    if (tty->signal_eof) {
        tty->signal_eof = false;
        return 0;
    }

    return ring_read(&tty->ring, data, len);
}

int tty_ioctl(file *file, int request, void *argp)
{
    tty *tty = file_tty(file);
    if (IS_ERROR(tty))
        return ERROR(tty);

    switch (request) {
    case TTY_SETPGRP:
        tty->controlling_pgrp = (intptr_t)argp;
        return 0;
    case TTY_SETBUFFER:
        tty->buffer_mode = static_cast<tty_buffering_mode>((intptr_t)argp);
        return 0;
    case TTY_SETECHO:
        tty->echo = (intptr_t)argp;
        return 0;
    case TTY_ISTTY:
        return 1;
    }
    return -EINVAL;
}

file_operations tty_ops = {
    .read = tty_read,
    .write = tty_write,
    .ioctl = tty_ioctl,
};
