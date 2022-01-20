#pragma once
#ifndef NG_NEWMUTEX_H
#define NG_NEWMUTEX_H

#include <basic.h>
#include <stdatomic.h>
#include <ng/thread.h>

struct newmutex {
    // lock == 0, nothing reading or writing.
    // lock == -N, locked write, (N-1) waiting
    // lock == +N, locked read, (N-1) reading
    atomic_int lock;
    atomic_int ticket;
    atomic_int waiting;
    int id;
};

typedef struct newmutex newmutex_t;

void newmutex_init(newmutex_t *newmutex);
bool newmutex_trylock(newmutex_t *newmutex);
int newmutex_lock(newmutex_t *newmutex);
int newmutex_unlock(newmutex_t *newmutex);

#endif
