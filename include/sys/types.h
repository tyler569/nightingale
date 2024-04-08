#pragma once

#include <limits.h>
#include <stdint.h>

typedef uintptr_t size_t;
typedef intptr_t ssize_t;

typedef uintptr_t virt_addr_t;
typedef uintptr_t phys_addr_t;

typedef int pid_t;
typedef int64_t off_t;
typedef int uid_t;
typedef int gid_t;
typedef unsigned dev_t;
typedef int64_t ino_t;
typedef int mode_t;
typedef int nlink_t;
typedef size_t blksize_t;
typedef size_t blkcnt_t;
typedef int64_t time_t;
typedef size_t socklen_t;
typedef int nfds_t;

typedef int clone_fn(void *);

