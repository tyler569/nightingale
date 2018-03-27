
#include "ctype.h"

bool isalnum(char c) {
    return ((c >= '0') && (c <= '9')) ||
           ((c >= 'A') && (c <= 'Z')) ||
           ((c >= 'a') && (c <= 'z'));
}

bool isalpha(char c) {
    return ((c >= 'A') && (c <= 'Z')) ||
           ((c >= 'a') && (c <= 'z'));
}

bool islower(char c) {
    return ((c >= 'a') && (c <= 'z'));
}

bool isupper(char c) {
    return ((c >= 'A') && (c <= 'Z'));
}

bool isdigit(char c) {
    return ((c >= '0') && (c <= '9'));
}

bool isxdigit(char c) {
    return ((c >= '0') && (c <= '9')) ||
           ((c >= 'a') && (c <= 'f')) ||
           ((c >= 'A') && (c <= 'F'));
}

bool iscntrl(char c) {
    return (c <= 31 || c == 127);
}

bool isspace(char c) {
    return (c >= 9 && c <= 13) || c == ' '; // c in "\t\n\v\f\r "
}

bool isblank(char c) {
    return (c == 9 || c == ' ');
}

bool isprint(char c) {
    return (c > 31 && c < 127);
}

bool ispunct(char c) {
    return (c >= '!' && c <= '/') ||
           (c >= ':' && c <= '@') ||
           (c >= '[' && c <= '`') ||
           (c >= '{' && c <= '~');
}

