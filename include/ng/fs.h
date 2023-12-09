#pragma once
#ifndef NG_FS_H
#define NG_FS_H

#include <dirent.h>
#include <list.h>
#include <ng/fs/dentry.h>
#include <ng/fs/file.h>
#include <ng/fs/file_system.h>
#include <ng/fs/inode.h>
#include <ng/fs/proc.h>
#include <ng/ringbuf.h>
#include <ng/sync.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ng/tty.h>
#include <poll.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <sys/socket.h>
#include <sys/types.h>

#endif // NG_FS_H
