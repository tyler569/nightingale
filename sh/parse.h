#ifndef SH_PARSE_H
#define SH_PARSE_H

#include "sh.h"

struct sh_command *parse_line(struct vector *tokens, ssize_t index, int next_input);

#endif
