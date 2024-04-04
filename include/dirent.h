#pragma once
#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <errno.h>
#include <fcntl.h>
#include <sys/cdefs.h>
#include <sys/types.h>

BEGIN_DECLS

struct dirent {
	ino_t d_ino;
	off_t d_off;
	unsigned short d_reclen;
	unsigned short d_mode;
	unsigned char d_type;
	char d_name[256];
};

#ifndef __kernel__
ssize_t getdents(int fd, struct dirent *buf, size_t size);
ssize_t readdir(int fd, struct dirent *buf, size_t size);
#endif

END_DECLS

#endif // _DIRENT_H_
