
#pragma once
#ifndef _NX_STRING_HH_
#define _NX_STRING_HH_

#include <basic.h>
#include <iostream.hh>

namespace nx {

struct string {
    char *data;
    unsigned long len;
    unsigned long cap;
 
    string();
    string(const char *);
    string(string const&);
    string(string&&);
 
    unsigned long length() const {
        return len;
    }
 
    unsigned long capacity() const {
        return cap;
    }
 
    char *c_str() const {
        return data;
    }
 
    friend ostream& operator<<(ostream&, string const&);
};
 
struct string_view {
    const char *data;
    unsigned long len;
 
    string_view(const char *c);
    string_view(string s);
    string_view(string_view const&);

    unsigned long length() {
        return len;
    }
 
    friend ostream& operator<<(ostream&, string_view const&);
};

}

#endif // _NX_STRING_HH_

