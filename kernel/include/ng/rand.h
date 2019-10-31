
#pragma once
#ifndef NIGHTINGALE_RAND_H
#define NIGHTINGALE_RAND_H

#include <ng/basic.h>
#include <stddef.h>
#include <stdint.h>

void rand_add_entropy(uint64_t entropy);
int32_t rand_get();

#endif
