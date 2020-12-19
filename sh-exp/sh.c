#include <assert.h>
#include <ctype.h>
#include "list.h" // <list.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "parse.h"

int main() {
    char buffer[1024];
    list tokens;
    while (fgets(buffer, 1024, stdin)) {
        list_init(&tokens);
        if (tokenize(buffer, &tokens)) {
            list_for_each(struct token, t, &tokens, node) {
                token_print(t);
            }
            parse(&tokens);
        }
    }
}
