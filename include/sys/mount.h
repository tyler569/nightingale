#pragma once

enum filesystem_types {
    _FS_PROCFS = 1,
};

int mount(const char *target, int type, const char *source);
int mountat(int fd, const char *target, int type, int sfd, const char *source);
