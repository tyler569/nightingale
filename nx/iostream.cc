
#include <basic.h>
extern "C" {
#include <stdio.h>
#include <string.h>
}
#include <iostream.hh>

namespace nx {

void _cout_type::output(const char *buffer, int length) {
    printf("%s", buffer);
}
 
ostream& operator<<(ostream& s, int x) {
    char buffer[1024];
    int len = snprintf(buffer, 1024, "%i", x);
    s.output(buffer, len);
    return s;
}
 
ostream& operator<<(ostream& s, const char *str) {
    s.output(str, strlen(str));
    return s;
}

}

