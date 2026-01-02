#pragma once

#include "sys/cdefs.h"

BEGIN_DECLS

extern bool ignore_timer_interrupt;

int pit_create_periodic(int hz);
int pit_create_oneshot(int nanoseconds);
int pit_ignore();
uint16_t pit_read_count(void);

END_DECLS
