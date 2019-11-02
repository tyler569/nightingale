
#pragma once
#ifndef _NX_IOSTREAM_HH_
#define _NX_IOSTREAM_HH_

#include <basic.h>
extern "C" {
#include <stdio.h>
}

namespace nx {
 
struct ostream {
    virtual void output(const char *buffer, int length) = 0;
};
 
struct _cout_type : public ostream {
    virtual void output(const char *buffer, int length) override;
};

// Investigate if this makes any sense at all.
static _cout_type cout;

ostream& operator<<(ostream& s, int x);
ostream& operator<<(ostream& s, const char *str);
// TODO: more base types

}

#endif // _NX_IOSTREAM_HH_

