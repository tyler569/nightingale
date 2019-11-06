
#pragma once
#ifndef NG_X86_PIT_H
#define NG_X86_PIT_H

#include <basic.h>

bool ignore_timer_interrupt;

int pit_create_periodic(int hz);
int pit_create_oneshot(int nanoseconds);
int pit_ignore(void);

#endif // NG_X86_PIT_H

