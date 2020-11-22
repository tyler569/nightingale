#pragma once
#ifndef NG_UNAME_H
#define NG_UNAME_H

#define UNAME_STR_LEN 65

struct utsname {
    char sysname[UNAME_STR_LEN];
    char nodename[UNAME_STR_LEN];
    char release[UNAME_STR_LEN];
    char version[UNAME_STR_LEN];
    char machine[UNAME_STR_LEN];
};

#endif // NG_UNAME_H
