
#include <ng/basic.h>
#include <ng/ringbuf.h>
#include <ng/fs.h>
#include <ng/thread.h>
#include <ng/debug.h>
#include <ng/panic.h>
#include <ng/serial.h>
#include <ng/syscall.h>
#include <ng/print.h>
#include <ng/tty.h>

struct tty serial_tty = {0};

void init_serial_tty(void) {
        serial_tty.initialized = 1;
        serial_tty.push_threshold = 256;
        serial_tty.buffer_index = 0;
        serial_tty.device_file = fs_resolve_relative_path(NULL, "/dev/serial");
        serial_tty.buffer_mode = 1;
        serial_tty.echo = 1;
}

int write_to_serial_tty(char c) {
        if (!serial_tty.initialized) {
                // TODO: race condition with startup - what happens if you
                // type before the vfs has been populated?
                init_serial_tty();
        }

        if (c == '\r') {
                serial_tty.buffer[serial_tty.buffer_index++] = '\n';

                ring_write(&serial_tty.device_file->extra.ring,
                           serial_tty.buffer, serial_tty.buffer_index);
                if (serial_tty.echo) {
                        serial_write('\r');
                        serial_write('\n');
                }
                serial_tty.buffer_index = 0;

                wake_blocked_threads(&serial_tty.device_file->blocked_threads);
        } else if (c == '\030') { // ^X
                // very TODO:
                // send_signal(serial_tty.controlling_pgrp, INT);
                struct process *p = process_by_id(serial_tty.controlling_pgrp);
                if (!p || p->pid == 0) {
                        printf("Controlling process %i is invalid\n",
                                        serial_tty.controlling_pgrp);
                } else {
                        // TODO:
                        // kill_process(p);

                        p->status = 1;
                }
                wake_blocked_threads(&serial_tty.device_file->blocked_threads);
        } else if (c == '\004') { // ^D
                serial_tty.device_file->signal_eof = 1;
                wake_blocked_threads(&serial_tty.device_file->blocked_threads);
        } else if (serial_tty.buffer_mode == 0) {
                serial_tty.buffer[serial_tty.buffer_index++] = c;

                ring_write(&serial_tty.device_file->extra.ring,
                           serial_tty.buffer, serial_tty.buffer_index);
                if (serial_tty.echo)  serial_write(c);
                serial_tty.buffer_index = 0;

                wake_blocked_threads(&serial_tty.device_file->blocked_threads);
        } else if (c >= ' ' && c <= '~') {
                if (serial_tty.echo)  serial_write(c);
                serial_tty.buffer[serial_tty.buffer_index++] = c;
        } else if (c < ' ') {
                if (serial_tty.echo) {
                        serial_write('^');
                        serial_write('@' + c);
                }
        } else if (c == '\177') {
                if (serial_tty.buffer_index) {
                        serial_tty.buffer[serial_tty.buffer_index] = '\0';
                        serial_tty.buffer_index -= 1;
                        if (serial_tty.echo) {
                                serial_write('\b');
                                serial_write(' ');
                                serial_write('\b');
                        }
                }
        } else {
                serial_write('?');
        }

        return 0;
}

sysret sys_ttyctl(int fd, int cmd, int arg) {
        struct open_fd *ofd = dmgr_get(&running_process->fds, fd);
        if (ofd == NULL)  return error(EBADF);
        struct fs_node *node = ofd->node;
        if (node == NULL || node->filetype != TTY)  return error(EINVAL);
        struct tty *t = node->extra.tty;

        assert(t == &serial_tty, "There should only be one serial_tty");

        if (cmd == TTY_SETPGRP) {
                t->controlling_pgrp = arg;
        } else if (cmd == TTY_SETBUFFER) {
                t->buffer_mode = arg;
        } else if (cmd == TTY_SETECHO) {
                t->echo = arg;
        }

        return value(0);
}

