
#ifndef NIGHTINGALE_DMGR_H
#define NIGHTINGALE_DMGR_H

#include <basic.h>

struct dmgr_element {
    int handle;
    void* pointer;
};

struct dmgr {
    int cap, len, full, first_free;
    struct dmgr_element* data;
};

void dmgr_init(struct dmgr* d);
int dmgr_insert(struct dmgr* d, void* ptr);
void* dmgr_get(struct dmgr* d, int handle);
void* dmgr_drop(struct dmgr* d, int handle);

#endif

