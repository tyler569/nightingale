#pragma once

enum tty_ioctls {
    TTY_SETPGRP,
    TTY_SETBUFFER,
    TTY_SETECHO,
    TTY_ISTTY,
};

#if !defined(__kernel__) && !defined(IOCTL_NO_IOCTL)
// see fcntl for reasonning - same applies to open(3)
int ioctl(int fd, int request, ...);
#endif
