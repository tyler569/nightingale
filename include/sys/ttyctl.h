#pragma once
#ifndef _SYS_TTYCTL_H_
#define _SYS_TTYCTL_H_

#include <sys/cdefs.h>
#include <sys/ioctl.h>

BEGIN_DECLS

int ttyctl(int fd, enum tty_ioctls cmd, int arg);

END_DECLS

#endif // _SYS_TTYCTL_H_
