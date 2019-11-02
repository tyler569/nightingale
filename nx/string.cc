
#include <basic.h>
extern "C" {
#include <string.h>
}
#include <string.hh>

namespace nx {

string::string() {
    len = 0;
    cap = 32;
    data = new char[cap];
}

string::string(const char *c) {
    len = strlen(c);
    data = new char[len];
    cap = len;
    memcpy(data, c, len);
}
 
string::string(string const& c) {
    len = c.length();
    data = new char[len];
    cap = len;
    memcpy(data, c.c_str(), len);
}

string::string(string&& c) {
    len = c.len;
    data = c.data;
}

ostream& operator<<(ostream& s, string const& v) {
    s << v.c_str();
    return s;
}
 

string_view::string_view(const char *c) {
    data = c;
    len = strlen(c);
}

string_view::string_view(string s) {
    data = s.c_str();
    len = s.length();
}

string_view::string_view(string_view const& s) {
    data = s.data;
    len = s.len;
}

ostream& operator<<(ostream& s, string_view const& v) {
    s << v.data;
    return s;
}

}

