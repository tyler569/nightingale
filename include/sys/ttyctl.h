#pragma once

#include <sys/cdefs.h>
#include <sys/ioctl.h>

BEGIN_DECLS

int ttyctl(int fd, enum tty_ioctls cmd, int arg);

END_DECLS

