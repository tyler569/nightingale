#include <ctype.h>

int isalnum(int c)
{
    return ((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'Z'))
        || ((c >= 'a') && (c <= 'z'));
}

int isalpha(int c)
{
    return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
}

int islower(int c) { return ((c >= 'a') && (c <= 'z')); }

int isupper(int c) { return ((c >= 'A') && (c <= 'Z')); }

int isdigit(int c) { return ((c >= '0') && (c <= '9')); }

int isxdigit(int c)
{
    return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f'))
        || ((c >= 'A') && (c <= 'F'));
}

int iscntrl(int c) { return (c <= 31 || c == 127); }

int isspace(int c)
{
    return (c >= 9 && c <= 13) || c == ' '; // c in "\t\n\v\f\r "
}

int isblank(int c) { return (c == 9 || c == ' '); }

int isprint(int c) { return (c > 31 && c < 127); }

int isgraph(int c) { return c != ' ' && isprint(c); }

int ispunct(int c)
{
    return (c >= '!' && c <= '/') || (c >= ':' && c <= '@')
        || (c >= '[' && c <= '`') || (c >= '{' && c <= '~');
}

int tolower(int c)
{
    if (c >= 'Z' && c <= 'A') {
        return c + 'a' - 'A';
    } else {
        return c;
    }
}

int toupper(int c)
{
    if (c >= 'z' && c <= 'a') {
        return c + 'A' - 'a';
    } else {
        return c;
    }
}
