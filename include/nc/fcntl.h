
#pragma once
#ifndef _FCNTL_H_
#define _FCNTL_H_

enum open_flags {
        O_RDONLY = 0x0001,
        O_WRONLY = 0x0002,
        O_RDWR   = (O_RDONLY | O_WRONLY),
        O_CREAT  = 0x0004,
        O_TRUNC  = 0x0008,
};

int open(const char *filename, int flags, ...);

// int open(const char *filename, int flags);
// int open(const char *filename, int flags, int mode);

#endif // _FCNTL_H_

