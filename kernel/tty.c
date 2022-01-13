#include <basic.h>
#include <errno.h>
#include <ng/debug.h>
#include <ng/fs.h>
#include <ng/panic.h>
#include <ng/ringbuf.h>
#include <ng/serial.h>
#include <ng/signal.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/tty.h>
#include <stdio.h>

ssize_t dev_serial_write(struct open_file *ofd, const void *data, size_t len) {
    struct file *file = ofd->file;
    assert(file->type == FT_TTY);
    struct tty_file *tty_file = (struct tty_file *)file;

    tty_file->tty.print_fn(data, len);
    return len;
}

ssize_t dev_serial_read(struct open_file *n, void *data_, size_t len) {
    struct file *file = n->file;
    assert(file->type == FT_TTY);
    struct tty_file *tty_file = (struct tty_file *)file;

    char *data = data_;

    ssize_t count = ring_read(&tty_file->tty.ring, data, len);

    if (count == 0) return -1;

    return count;
}

struct file_ops dev_serial_ops = {
    .read = dev_serial_read,
    .write = dev_serial_write,
};

struct tty_file dev_serial = {
    .file =
        {
            .ops = &dev_serial_ops,
            .type = FT_TTY,
            .mode = USR_READ | USR_WRITE,
        },
    .tty =
        {
            .push_threshold = 256,
            .buffer_index = 0,
            .buffer_mode = 1,
            .echo = 1,
            .print_fn = serial_write_str,
        },
};

struct tty_file dev_serial2 = {
    .file =
        {
            .ops = &dev_serial_ops,
            .type = FT_TTY,
            .mode = USR_READ | USR_WRITE,
        },
    .tty =
        {
            .push_threshold = 256,
            .buffer_index = 0,
            .buffer_mode = 1,
            .echo = 1,
            .print_fn = serial_write_str,
        },
};

#define CONTROL(c) ((c) - 'a' + 1)
int pm_avail(void);

int write_to_serial_tty(struct tty_file *tty_file, char c) {
    struct tty *serial_tty = &tty_file->tty;
    struct file *file = &tty_file->file;

    if (!serial_tty->initialized) {
        emplace_ring(&serial_tty->ring, 256);
        serial_tty->initialized = true;
    }

    if (c == '\r' || c == '\n') {
        serial_tty->buffer[serial_tty->buffer_index++] = '\n';

        ring_write(&serial_tty->ring, serial_tty->buffer,
                   serial_tty->buffer_index);
        if (serial_tty->echo) serial_tty->print_fn("\r\n", 2);
        serial_tty->buffer_index = 0;

        wq_notify_all(&file->readq);
    } else if (c == CONTROL('c') || c == CONTROL('x')) {
        // preliminary signal interrupt
        // ^X is allowed becasue ^C will terminate the VM in serial mode.
        signal_send_pgid(serial_tty->controlling_pgrp, SIGINT);
    } else if (c == CONTROL('t')) {
        // VSTATUS
        print_cpu_info(); // TODO: send to TTY, not kernel serial terminal
        // signal_send_pgid(serial_tty->controlling_pgrp, SIGINFO);
    } else if (c == CONTROL('o')) {
        // debug output available physical memory
        // TODO: send to TTY, not kernel serial terminal
        printf("mem avail: %i (%ik)\n", pm_avail(), pm_avail()/1024);
    } else if (c == CONTROL('p')) {
        // debug event_log status
        extern int bytes_written;
        printf("bytes written: %i\n", bytes_written);
    } else if (c == CONTROL('d')) {
        file->signal_eof = 1;
        wq_notify_all(&file->readq);
    } else if (serial_tty->buffer_mode == 0) {
        serial_tty->buffer[serial_tty->buffer_index++] = c;

        ring_write(&serial_tty->ring, serial_tty->buffer,
                   serial_tty->buffer_index);
        if (serial_tty->echo) serial_tty->print_fn(&c, 1);
        serial_tty->buffer_index = 0;

        wq_notify_all(&file->readq);
    } else if (c >= ' ' && c <= '~') {
        if (serial_tty->echo) serial_tty->print_fn(&c, 1);
        serial_tty->buffer[serial_tty->buffer_index++] = c;
    } else if (c < ' ') {
        if (serial_tty->echo) {
            serial_tty->print_fn("^", 1);
            char ctrl = '@' + c;
            serial_tty->print_fn(&ctrl, 1);
        }
    } else if (c == '\177') {
        if (serial_tty->buffer_index) {
            serial_tty->buffer[serial_tty->buffer_index] = '\0';
            serial_tty->buffer_index -= 1;
            if (serial_tty->echo) serial_tty->print_fn("\b \b", 3);
        }
    } else {
        serial_tty->print_fn("?", 1);
    }

    return 0;
}

sysret sys_ttyctl(int fd, int cmd, int arg) {
    struct open_file *ofd = dmgr_get(&running_process->fds, fd);
    if (ofd == NULL) return -EBADF;
    struct file *file = ofd->file;
    struct tty *t = NULL;

    if (file->type == FT_TTY) {
        struct tty_file *tty_file = (struct tty_file *)file;
        t = &tty_file->tty;
    }

    switch (cmd) {
    case TTY_SETPGRP:
        if (!t) return -ENOTTY;
        t->controlling_pgrp = arg;
        break;
    case TTY_SETBUFFER:
        if (!t) return -ENOTTY;
        t->buffer_mode = arg;
        break;
    case TTY_SETECHO:
        if (!t) return -ENOTTY;
        t->echo = arg;
        break;
    case TTY_ISTTY: return ofd->file->type == FT_TTY;
    default: return -EINVAL;
    }

    return 0;
}
