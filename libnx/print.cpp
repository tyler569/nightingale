#include <nx/print.h>
#include <nx/string.h>
#include <stdio.h>

namespace nx {

template <> void print(const string &arg) { printf("%s", arg.c_str()); }
template <> void print(const string_view &arg) { printf("%s", arg.c_str()); }
template <> void print(const char *arg) { printf("%s", arg); }
template <> void print(const char &arg) { printf("%c", arg); }
template <> void print(const int &arg) { printf("%d", arg); }
template <> void print(const unsigned int &arg) { printf("%u", arg); }
template <> void print(const long &arg) { printf("%ld", arg); }
template <> void print(const unsigned long &arg) { printf("%lu", arg); }
template <> void print(const long long &arg) { printf("%lld", arg); }
template <> void print(const unsigned long long &arg) { printf("%llu", arg); }
template <> void print(const void *arg) { printf("%p", arg); }
template <> void print(const bool &arg)
{
    printf("%s", arg ? "true" : "false");
}

}