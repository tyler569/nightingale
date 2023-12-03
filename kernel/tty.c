#include "ng/tty.h"
#include "ng/debug.h"
#include "ng/fs/file.h"
#include "ng/panic.h"
#include "ng/ringbuf.h"
#include "ng/serial.h"
#include "ng/signal.h"
#include "ng/syscall.h"
#include "ng/thread.h"
#include <basic.h>
#include <errno.h>
#include <stdio.h>

#define CONTROL(c) ((c) - 'a' + 1)
#define NO_BUFFERING 0
#define BUFFERING 1

struct tty *global_ttys[32];

// move me
void tty_init(void)
{
    extern struct serial_device *x86_com[2];
    new_tty(x86_com[0], 0);
    new_tty(x86_com[1], 1);
}

struct tty *new_tty(struct serial_device *dev, int id)
{
    struct tty *tty = malloc(sizeof(struct tty));

    *tty = (struct tty) {
        .push_threshold = 256,
        .buffer_mode = BUFFERING,
        .echo = true,
        .serial_device = dev,
    };

    wq_init(&tty->read_queue);

    ring_emplace(&tty->ring, 256);
    global_ttys[id] = tty;
    dev->tty = tty;

    return tty;
}

static void print_to_user(struct tty *tty, const char *c, size_t len)
{
    if (tty->echo && tty->serial_device)
        tty->serial_device->ops->write_string(tty->serial_device, c, len);
}

static void buffer_push(struct tty *tty, char c)
{
    tty->buffer[tty->buffer_index++] = c;
}

static void buffer_flush(struct tty *tty)
{
    ring_write(&tty->ring, tty->buffer, tty->buffer_index);
    tty->buffer_index = 0;
    wq_notify_all(&tty->read_queue);
}

int tty_push_byte(struct tty *tty, char c)
{
    if (c == '\r' || c == '\n' || c == CONTROL('m')) {
        buffer_push(tty, '\n');
        print_to_user(tty, "\r\n", 2);
        buffer_flush(tty);
    } else if (c == CONTROL('c') || c == CONTROL('x')) {
        // preliminary signal interrupt
        // ^X is allowed becasue ^C will terminate the VM in serial mode.
        signal_send_pgid(tty->controlling_pgrp, SIGINT);
    } else if (c == CONTROL('t')) {
        // VSTATUS
        print_cpu_info(); // TODO: send to TTY, not kernel serial terminal
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
