#pragma once
#ifndef _SYS_TTYCTL_H_
#define _SYS_TTYCTL_H_

#include <sys/ioctl.h>

int ttyctl(int fd, enum tty_ioctls cmd, int arg);

#endif // _SYS_TTYCTL_H_
