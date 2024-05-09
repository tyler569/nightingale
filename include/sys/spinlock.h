#pragma once

#include "stdatomic.h"

struct spin_lock {
	atomic_int front, back;
};

typedef struct spin_lock spin_lock_t;

void spin_lock(spin_lock_t *lock);
void spin_unlock(spin_lock_t *lock);
