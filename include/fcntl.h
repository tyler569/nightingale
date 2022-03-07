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
};

enum file_type {
    FT_NORMAL,
    FT_CHAR_DEV,
    FT_TTY,
    FT_BUFFER,
    FT_SOCKET,
    FT_DIRECTORY,
    FT_PIPE,
    FT_PROC,
    FT_SYMLINK,
    FT_BLOCK,
};

#define _NG_CHAR_DEV   (FT_CHAR_DEV << 16)
#define _NG_DIR        (FT_DIRECTORY << 16)
#define _NG_SYMLINK    (FT_SYMLINK << 16)

#define S_IFREG         (FT_NORMAL << 16)
#define S_IFCHR (FT_CHAR_DEV << 16)
#define S_IFBLK (FT_BLOCK << 16)
#define S_IFIFO (FT_PIPE << 16)
#define S_IFSOCK (FT_SOCKET << 16)

enum file_mode {
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

    ALL_READ = S_IROTH,
    ALL_WRITE = S_IWOTH,
    ALL_EXEC = S_IXOTH,
    GRP_READ = S_IRGRP,
    GRP_WRITE = S_IWGRP,
    GRP_EXEC = S_IXGRP,
    USR_READ = S_IRUSR,
    USR_WRITE = S_IWUSR,
    USR_EXEC = S_IXUSR,

    STICKY = S_ISVTX,
    SUID = S_ISUID,
    SGID = S_ISGID,
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
