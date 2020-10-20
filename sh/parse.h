#pragma once
#ifndef NGSH_PARSE_H
#define NGSH_PARSE_H

#include "sh.h"

struct sh_command *parse_line(struct vector *tokens, ssize_t index, int next_input);
void recursive_free_sh_command(struct sh_command *);

#endif // NGSH_PARSE_H
