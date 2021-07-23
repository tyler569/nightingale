#include <basic.h>
#include <ng/string.h>

int chr_in(char chr, const char *options) {
    for (size_t i = 0; options[i]; i++) {
        if (chr == options[i]) return 1;
    }

    return 0;
}

/*
 * Reads from source into tok until one of the characters in delims is
 * encountered.  Leaves a pointer to the rest of the string in rest.
 */
const char *str_until(const char *source, char *tok, const char *delims) {
    size_t i;
    const char *rest = NULL;

    for (i = 0; source[i]; i++) {
        if (chr_in(source[i], delims)) break;
        tok[i] = source[i];
    }

    tok[i] = 0;

    if (source[i]) rest = &source[i + 1];

    return rest;
}

char *strcpyto(char *dest, const char *source, char delim) {
    while (*source && *source != delim) { *dest++ = *source++; }
    *dest = 0;
    return (char *)source;
}

char *strncpyto(char *dest, const char *source, size_t len, char delim) {
    size_t n = 0;
    while (*source && *source != delim && n < len) {
        *dest++ = *source++;
        n++;
    }
    if (n < len) *dest = 0;
    return (char *)source;
}
