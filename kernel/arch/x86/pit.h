
#pragma once
#ifndef NIGHTINGALE_PIT_H
#define NIGHTINGALE_PIT_H

#include <basic.h>

int pit_create_periodic(int hz);
int pit_create_oneshot(int nanoseconds);

#endif
