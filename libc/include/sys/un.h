#pragma once
#ifndef _SYS_UN_H_
#define _SYS_UN_H_

#include <sys/socket.h>

struct sockaddr_un {
    sa_family_t sun_family;
    char sun_path[108];
};

#endif // _SYS_UN_H_
