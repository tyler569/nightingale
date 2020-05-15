
#include <basic.h>
#include <ng/ringbuf.h>
#include <ng/fs.h>
#include <ng/thread.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <ng/serial.h>
#include <ng/syscall.h>
#include <ng/tty.h>
#include <ng/signal.h>
#include <stdio.h>
#include <errno.h>

struct tty serial_tty = {0};
struct tty serial_tty2 = {0};

void serial_ttys_init(void) {
        if (serial_tty.initialized)  return;

        serial_tty.initialized = 1;
        serial_tty.push_threshold = 256;
        serial_tty.buffer_index = 0;
        serial_tty.device_file = fs_resolve_relative_path(NULL, "/dev/serial");
        serial_tty.buffer_mode = 1;
        serial_tty.echo = 1;
        serial_tty.print_fn = serial_write_str;
        printf("/dev/serial: tty ready\n");

        serial_tty2.initialized = 1;
        serial_tty2.push_threshold = 256;
        serial_tty2.buffer_index = 0;
        serial_tty2.device_file = fs_resolve_relative_path(NULL, "/dev/serial2");
        serial_tty2.buffer_mode = 1;
        serial_tty2.echo = 1;
        serial_tty2.print_fn = serial2_write_str;
        printf("/dev/serial2: tty ready\n");
}

int write_to_serial_tty(struct tty *serial_tty, char c) {
        if (!serial_tty->initialized) {
                // TODO: race condition with startup - what happens if you
                // type before the vfs has been populated?
                serial_ttys_init();
        }

        if (c == '\r' || c == '\n') {
                serial_tty->buffer[serial_tty->buffer_index++] = '\n';

                ring_write(&serial_tty->device_file->ring,
                           serial_tty->buffer, serial_tty->buffer_index);
                if (serial_tty->echo) {
                        serial_tty->print_fn("\r\n", 2);
                }
                serial_tty->buffer_index = 0;

                wake_blocked_threads(&serial_tty->device_file->blocked_threads);
        } else if (c == '\030' || c == '\003') { // ^X | ^C
                // very TODO:
                signal_send_pgid(serial_tty->controlling_pgrp, SIGINT);
        } else if (c == '\004') { // ^D
                serial_tty->device_file->signal_eof = 1;
                wake_blocked_threads(&serial_tty->device_file->blocked_threads);
        } else if (serial_tty->buffer_mode == 0) {
                serial_tty->buffer[serial_tty->buffer_index++] = c;

                ring_write(&serial_tty->device_file->ring,
                           serial_tty->buffer, serial_tty->buffer_index);
                if (serial_tty->echo)  serial_tty->print_fn(&c, 1);
                serial_tty->buffer_index = 0;

                wake_blocked_threads(&serial_tty->device_file->blocked_threads);
        } else if (c >= ' ' && c <= '~') {
                if (serial_tty->echo)  serial_tty->print_fn(&c, 1);
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
                        if (serial_tty->echo) {
                                serial_tty->print_fn("\b \b", 3);
                        }
                }
        } else {
                serial_tty->print_fn("?", 1);
        }

        return 0;
}

sysret sys_ttyctl(int fd, int cmd, int arg) {
        struct open_file *ofd = dmgr_get(&running_process->fds, fd);
        if (ofd == NULL)  return -EBADF;
        struct file *node = ofd->node;

        struct tty *t = node->tty;

        switch (cmd) {
        case TTY_SETPGRP: 
                if (!t)  return -EINVAL;
                t->controlling_pgrp = arg;
                break;
        case TTY_SETBUFFER:
                if (!t)  return -EINVAL;
                t->buffer_mode = arg;
                break;
        case TTY_SETECHO:
                if (!t)  return -EINVAL;
                t->echo = arg;
                break;
        case TTY_ISTTY:
                return ofd->node->filetype == FT_TTY;
        default:
                return -EINVAL;
        }

        return 0;
}

