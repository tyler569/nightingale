#ifndef SH_SH_H
#define SH_SH_H

struct sh_command {
    struct sh_command *next;

    char **args;
    char *arg_buf;
    int output;
    int input;
};

#endif
