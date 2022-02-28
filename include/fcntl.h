#pragma once
#ifndef _FCNTL_H_
#define _FCNTL_H_

enum open_flags {
    O_RDONLY = 0x01,
    O_WRONLY = 0x02,
    O_RDWR = O_RDONLY | O_WRONLY,
    O_APPEND = 0x04,
    O_ASYNC = 0x08,
    O_CLOEXEC = 0x10,
    O_NONBLOCK = 0x20,
    O_NDELAY = O_NONBLOCK,
    O_CREAT = 0x40,
    O_TRUNC = 0x80,
    O_EXCL = 0x100,

    _NG_DIR = 0x10000,
};

enum file_type {
    FT_NORMAL,
    FT_CHARDEV,
    FT_TTY,
    FT_BUFFER,
    FT_SOCKET,
    FT_DIRECTORY,
    FT_PIPE,
    FT_PROC,
};

enum file_mode {
    ALL_READ = 0004,
    ALL_WRITE = 0002,
    ALL_EXEC = 0001,
    GRP_READ = 0040,
    GRP_WRITE = 0020,
    GRP_EXEC = 0010,
    USR_READ = 0400,
    USR_WRITE = 0200,
    USR_EXEC = 0100,

    STICKY = 01000,
    SUID = 02000,
    SGID = 04000,

    S_ISUID = 04000, // set-user-ID bit
    S_ISGID = 02000, // set-group-ID bit
    S_ISVTX = 01000, // sticky bit

    S_IRWXU = 00700, // owner has read, write, and execute permission
    S_IRUSR = 00400, // owner has read permission
    S_IWUSR = 00200, // owner has write permission
    S_IXUSR = 00100, // owner has execute permission

    S_IRWXG = 00070, // group has read, write, and execute permission
    S_IRGRP = 00040, // group has read permission
    S_IWGRP = 00020, // group has write permission
    S_IXGRP = 00010, // group has execute permission

    S_IRWXO = 00007, // others (not in group) have read, write, and execute permission
    S_IROTH = 00004, // others have read permission
    S_IWOTH = 00002, // others have write permission
    S_IXOTH = 00001, // others have execute permission
};

enum {
    AT_FDCWD = -2,
};

// open(3) is defined with the ellipsis operator, but open(2) is not.
// This means the header declaration of open(3) cannot be visible in the
// TU there open(2) is instantiated - libc/syscalls.c
//
// I intend to fix this later, but this hack gets us compiling for now
#ifndef FCNTL_NO_OPEN
int open(const char *filename, int flags, ...);
#endif

// int open(const char *filename, int flags);
// int open(const char *filename, int flags, int mode);

#endif // _FCNTL_H_
