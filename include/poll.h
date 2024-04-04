#pragma once
#ifndef _POLL_H_
#define _POLL_H_

#include <sys/cdefs.h>

BEGIN_DECLS

struct pollfd {
	int fd;
	short events;
	short revents;
};

enum {
	POLLIN,
};

typedef int nfds_t;

int poll(struct pollfd *pollfds, nfds_t nfds, int timeout);

END_DECLS

#endif // _POLL_H_
