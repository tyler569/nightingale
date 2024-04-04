#pragma once

#include "sys/cdefs.h"
#include <stdbool.h>

BEGIN_DECLS

extern bool ignore_timer_interrupt;

int pit_create_periodic(int hz);
int pit_create_oneshot(int nanoseconds);
int pit_ignore(void);

END_DECLS

