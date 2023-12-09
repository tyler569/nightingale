#include <ng/debug.h>
#include <ng/ringbuf.h>
#include <ng/serial.h>
#include <ng/signal.h>
#include <ng/thread.h>
#include <ng/tty.h>

struct tty *global_ttys[32];

static constexpr char control(char c) { return c - 'a' + 1; }

static constexpr char control_vis(char c) { return c + '@'; }

// move me
void tty_init(void)
{
    extern struct serial_device *x86_com[2];
    new_tty(x86_com[0], 0);
    new_tty(x86_com[1], 1);
}

struct tty *new_tty(struct serial_device *dev, int id)
{
    auto *t = new tty {
        .push_threshold = 256,
        .serial_device = dev,
        .buffer_mode = TTY_BUFFERING,
        .echo = true,
    };

    wq_init(&t->read_queue);

    ring_emplace(&t->ring, 256);
    global_ttys[id] = t;
    dev->tty = t;

    return t;
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
    if (c == '\r' || c == '\n') {
        buffer_push(tty, '\n');
        print_to_user(tty, "\r\n", 2);
        buffer_flush(tty);
    } else if (c == control('c') || c == control('x')) {
        // preliminary signal interrupt
        // ^X is allowed becasue ^C will terminate the VM in serial mode.
        signal_send_pgid(tty->controlling_pgrp, SIGINT);
    } else if (c == control('t')) {
        // VSTATUS
        print_cpu_info(); // TODO: send to TTY, not kernel serial terminal
        signal_send_pgid(tty->controlling_pgrp, SIGINFO);
    } else if (c == control('d')) {
        if (tty->buffer_index > 0) {
            buffer_flush(tty);
        } else {
            tty->signal_eof = true;
            wq_notify_all(&tty->read_queue);
        }
    } else if (tty->buffer_mode == TTY_NO_BUFFERING) {
        buffer_push(tty, c);
        print_to_user(tty, "\r\n", 2);
        buffer_flush(tty);
    } else if (c >= ' ' && c <= '~') {
        buffer_push(tty, c);
        print_to_user(tty, &c, 1);
    } else if (c < ' ') {
        print_to_user(tty, "^", 1);
        char ctrl = control_vis(c);
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
