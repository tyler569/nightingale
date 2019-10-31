
#include <ng/basic.h>
#include <ng/panic.h>
#include <ng/string.h>

int chr_in(char chr, char *options) {
        for (size_t i=0; options[i]; i++) {
                if (chr == options[i])
                        return 1;
        }

        return 0;
}

/*
 * Reads from source into tok until one of the charcters in delims is
 * encountered.  Leaves a pointer to the rest of the string in rest.
 */
const char *str_until(const char *source, char *tok, char *delims) {
        size_t i;
        const char *rest = NULL;

        for (i=0; source[i]; i++) {
                if (chr_in(source[i], delims))
                        break;
                tok[i] = source[i];
        }

        tok[i] = 0;

        if (source[i])
                rest = &source[i+1];

        return rest;
}

