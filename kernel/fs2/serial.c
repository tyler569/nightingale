#include <basic.h>
#include <errno.h>
#include <ng/tty.h>
#include <ng/serial.h>
#include "file.h"
#include "inode.h"

#define wait_on wq_block_on
// #define wake_from wq_notify_all

static struct tty *file_tty(struct fs2_file *file) {
    int minor = file->inode->device_minor;
    if (minor > 32 || minor < 0)
        return TO_ERROR(-ENODEV);
    struct tty *tty = global_ttys[minor];
    if (!tty)
        return TO_ERROR(-ENODEV);
    return tty;
}

ssize_t tty_write(struct fs2_file *file, const char *data, size_t len) {
    struct tty *tty = file_tty(file);
    if (IS_ERROR(tty))
        return ERROR(tty);

    struct serial_device *dev = tty->serial_device;
    if (!dev)
        return -ENODEV; // ?
    
    dev->ops->write_string(dev, data, len);
    return len;
}

ssize_t tty_read(struct fs2_file *file, char *data, size_t len) {
    struct tty *tty = file_tty(file);
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
    return ring_read(&tty->ring, data, len);
}

struct file_operations tty_ops = {
    .read = tty_read,
    .write = tty_write,
};

