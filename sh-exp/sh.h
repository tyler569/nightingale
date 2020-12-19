#pragma once
#ifndef NGSH_SH_H
#define NGSH_SH_H

struct sh_command {
    struct sh_command *next;

    char **args;
    char *arg_buf;
    int output;
    int input;
};

#endif // NGSH_SH_H
