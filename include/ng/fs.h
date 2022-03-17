#pragma once
#ifndef NG_FS_H
#define NG_FS_H

#include <basic.h>
#include <ng/dmgr.h>
#include <ng/ringbuf.h>
#include <ng/sync.h>
#include <ng/syscall.h>
#include <ng/syscall_consts.h>
#include <ng/tty.h>
#include <dirent.h>
#include <list.h>
#include <poll.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../../kernel/fs2/file.h"
#include "../../kernel/fs2/inode.h"
#include "../../kernel/fs2/file_system.h"
#include "../../kernel/fs2/dentry.h"
#include "../../kernel/fs2/proc.h"

#endif // NG_FS_H
