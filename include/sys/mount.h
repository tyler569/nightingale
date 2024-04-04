#pragma once

#include <sys/cdefs.h>

BEGIN_DECLS

enum filesystem_types {
    _FS_PROCFS = 1,
};

int mount(const char *target, int type, const char *source);
int mountat(int fd, const char *target, int type, int sfd, const char *source);

END_DECLS

