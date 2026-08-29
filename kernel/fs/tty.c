#include <ng/fs/file.h>
#include <ng/fs/vnode.h>
#include <ng/serial.h>
#include <ng/tty.h>

static struct tty *file_tty(struct file *file) {
	int minor = file->vnode->device_minor;
	if (minor > 32 || minor < 0)
		return TO_ERROR(-ENODEV);
	struct tty *tty = global_ttys[minor];
	if (!tty)
		return TO_ERROR(-ENODEV);
	return tty;
}

ssize_t tty_write(struct file *file, const char *data, size_t len) {
	struct tty *tty = file_tty(file);
	if (IS_ERROR(tty))
		return ERROR(tty);

	struct serial_device *dev = tty->serial_device;
	if (!dev)
		return -ENODEV; // ?

	dev->ops->write_string(dev, data, len);
	return len;
}

ssize_t tty_read(struct file *file, char *data, size_t len) {
	struct tty *tty = file_tty(file);
	if (IS_ERROR(tty))
		return ERROR(tty);

	spin_lock(&tty->guard);

	size_t n_read = 0;

	while (true) {
		if (tty->signal_eof) {
			tty->signal_eof = false;
			break;
		}

		n_read = ring_read(&tty->ring, data, len);

		if (n_read)
			break;

		wq_wait(&tty->read_queue, &tty->guard);
	}

	spin_unlock(&tty->guard);
	return n_read;
}

int tty_ioctl(struct file *file, int request, void *argp) {
	struct tty *tty = file_tty(file);
	if (IS_ERROR(tty))
		return ERROR(tty);

	switch (request) {
	case TTY_SETPGRP:
		tty->controlling_pgrp = (intptr_t)argp;
		return 0;
	case TTY_SETBUFFER:
		tty->buffer_mode = (intptr_t)argp;
		return 0;
	case TTY_SETECHO:
		tty->echo = (intptr_t)argp;
		return 0;
	case TTY_ISTTY:
		return 1;
	}
	return -EINVAL;
}

struct file_ops tty_ops = {
	.read = tty_read,
	.write = tty_write,
	.ioctl = tty_ioctl,
};
