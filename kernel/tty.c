#include <basic.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <ng/ringbuf.h>
#include <ng/serial.h>
#include <ng/signal.h>
#include <ng/syscall.h>
#include <ng/thread.h>
#include <ng/tty.h>
#include <errno.h>
#include <stdio.h>

#include "fs2/file.h"

#define CONTROL(c) ((c) - 'a' + 1)
#define NO_BUFFERING 0
#define BUFFERING 1

struct tty *global_ttys[32];

struct tty *new_tty(struct serial_device *dev, int id) {
    struct tty *tty = malloc(sizeof(struct tty));

    *tty = (struct tty) {
        .push_threshold = 256,
        .buffer_mode = BUFFERING,
        .echo = true,
        .serial_device = dev,
    };

    emplace_ring(&tty->ring, 256);
    global_ttys[id] = tty;

    return tty;
}

static void print_to_user(struct tty *tty, const char *c, size_t len) {
    if (tty->echo && tty->serial_device)
        tty->serial_device->ops->write_string(tty->serial_device, c, len);
}

static void buffer_push(struct tty *tty, char c) {
    tty->buffer[tty->buffer_index++] = c;
}

static void buffer_flush(struct tty *tty) {
    ring_write(&tty->ring, tty->buffer, tty->buffer_index);
    tty->buffer_index = 0;
    wq_notify_all(&tty->read_queue);
}

int tty_push_byte(struct tty *tty, char c) {
    if (c == '\r' || c == '\n') {
        buffer_push(tty, c);
        print_to_user(tty, "\r\n", 2);
        buffer_flush(tty);
    } else if (c == CONTROL('c') || c == CONTROL('x')) {
        // preliminary signal interrupt
        // ^X is allowed becasue ^C will terminate the VM in serial mode.
        signal_send_pgid(tty->controlling_pgrp, SIGINT);
    } else if (c == CONTROL('t')) {
        // VSTATUS
        print_cpu_info();      // TODO: send to TTY, not kernel serial terminal
        signal_send_pgid(tty->controlling_pgrp, SIGINFO);
    } else if (c == CONTROL('d')) {
        if (tty->buffer_index > 0) {
            buffer_flush(tty);
        } else {
            tty->signal_eof = 1;
            wq_notify_all(&tty->read_queue);
        }
    } else if (tty->buffer_mode == NO_BUFFERING) {
        buffer_push(tty, c);
        print_to_user(tty, "\r\n", 2);
        buffer_flush(tty);
    } else if (c >= ' ' && c <= '~') {
        buffer_push(tty, c);
        print_to_user(tty, &c, 1);
    } else if (c < ' ') {
        print_to_user(tty, "^", 1);
        char ctrl = '@' + c;
        print_to_user(tty, &ctrl, 1);
    } else if (c == '\177') {
        if (tty->buffer_index) {
            tty->buffer[tty->buffer_index] = '\0';
            tty->buffer_index -= 1;
            print_to_user(tty, "\b \b", 3);
        }
    } else {
        print_to_user(tty, "?", 1);
    }

    return 0;
}

sysret sys_ttyctl(int fd, int cmd, int arg) {
    struct open_file *ofd = dmgr_get(&running_process->fds, fd);
    if (ofd == NULL)
        return -EBADF;
    struct file *file = ofd->file;
    struct tty *t = NULL;

    if (file->type == FT_TTY) {
        struct tty_file *tty_file = (struct tty_file *)file;
        t = &tty_file->tty;
    }

    switch (cmd) {
    case TTY_SETPGRP:
        if (!t)
            return -ENOTTY;
        t->controlling_pgrp = arg;
        break;
    case TTY_SETBUFFER:
        if (!t)
            return -ENOTTY;
        t->buffer_mode = arg;
        break;
    case TTY_SETECHO:
        if (!t)
            return -ENOTTY;
        t->echo = arg;
        break;
    case TTY_ISTTY:
        return ofd->file->type == FT_TTY;
    default:
        return -EINVAL;
    }

    return 0;
}
