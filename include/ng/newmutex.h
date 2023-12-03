#pragma once
#ifndef NG_NEWMUTEX_H
#define NG_NEWMUTEX_H

#include <basic.h>
#include <stdatomic.h>
#include <sys/cdefs.h>

BEGIN_DECLS

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

newmutex_t make_newmutex(void);
void newmutex_init(newmutex_t *newmutex);
bool newmutex_trylock(newmutex_t *newmutex);
int newmutex_lock(newmutex_t *newmutex);
int newmutex_unlock(newmutex_t *newmutex);

void wait_on_newmutex(newmutex_t *newmutex);
void wait_on_newmutex_cv(newmutex_t *condvar, newmutex_t *mutex);
void wake_awaiting_thread(newmutex_t *newmutex);
void wake_all_awaiting_threads(newmutex_t *newmutex);

END_DECLS

#endif // NG_NEWMUTEX_H
