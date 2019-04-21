
#pragma once
#ifndef NIGHTINGALE_PIT_H
#define NIGHTINGALE_PIT_H

#include <ng/basic.h>

bool ignore_timer_interrupt;

int pit_create_periodic(int hz);
int pit_create_oneshot(int nanoseconds);
int pit_ignore(void);

#endif
