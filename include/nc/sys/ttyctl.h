
#pragma once
#ifndef _SYS_TTYCTL_H_
#define _SYS_TTYCTL_H_

enum tty_ttyctls {
        TTY_SETPGRP,
        TTY_SETBUFFER,
        TTY_SETECHO,
        TTY_ISTTY,
};

int ttyctl(int fd, enum tty_ttyctls cmd, int arg);


#endif // _SYS_TTYCTL_H_

