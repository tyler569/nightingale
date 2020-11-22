#pragma once
#ifndef _SYS_UTSNAME_H_
#define _SYS_UTSNAME_H_

#define UNAME_STR_LEN 65

struct utsname {
    char sysname[UNAME_STR_LEN];
    char nodename[UNAME_STR_LEN];
    char release[UNAME_STR_LEN];
    char version[UNAME_STR_LEN];
    char machine[UNAME_STR_LEN];
};

int uname(struct utsname *);

#endif // _SYS_UTSNAME_H_
