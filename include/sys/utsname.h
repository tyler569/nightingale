#pragma once
#ifndef _SYS_UTSNAME_H_
#define _SYS_UTSNAME_H_

#include <sys/cdefs.h>

#define UNAME_STR_LEN 64

BEGIN_DECLS

struct utsname {
	char sysname[UNAME_STR_LEN];
	char nodename[UNAME_STR_LEN];
	char release[UNAME_STR_LEN];
	char version[UNAME_STR_LEN];
	char machine[UNAME_STR_LEN];
};

int uname(struct utsname *);

END_DECLS

#endif // _SYS_UTSNAME_H_
